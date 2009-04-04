/* simu.host.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */
#include "common.h"
#include "simu.host.h"

#include "modules/host/host.h"
#include "modules/host/mex.h"
#include "modules/utils/utils.h"
#include "modules/math/fixed/fixed.h"

#include "io.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pwm.h"
#include "aux.h"

#include "motor_model.host.h"
#include "models.host.h"

/** Simulate some AVR regs. */
uint8_t DDRF, PORTC, PORTD, PORTE, PORTF, PORTG, PINC;

/** Overall counter values. */
uint16_t counter_left, counter_right,
	 counter_aux[AC_ASSERV_AUX_NB];
/** Counter differences since last update.
 * Maximum of 9 significant bits, sign included. */
int16_t counter_left_diff, counter_right_diff,
	counter_aux_diff[AC_ASSERV_AUX_NB];
/** Overall uncorrected counter values. */
static int32_t counter_right_raw;
/** Correction factor (f8.24). */
uint32_t counter_right_correction = 1L << 24;

/** PWM control states. */
struct pwm_t pwm_left = PWM_INIT_FOR (pwm_left);
struct pwm_t pwm_right = PWM_INIT_FOR (pwm_right);
struct pwm_t pwm_aux[AC_ASSERV_AUX_NB] = {
    PWM_INIT_FOR (pwm_aux0), PWM_INIT_FOR (pwm_aux1)
};
/** PWM reverse directions. */
uint8_t pwm_reverse;

/* Robot model. */
const struct robot_t *simu_robot;

/** Motor models. */
struct motor_t simu_left_model, simu_right_model,
	       simu_aux_model[AC_ASSERV_AUX_NB];

/** Computed simulated position (mm, rad). */
double simu_pos_x, simu_pos_y, simu_pos_a;

/** Full counter values. */
uint32_t simu_counter_left, simu_counter_right,
	 simu_counter_aux[AC_ASSERV_AUX_NB];
double simu_counter_left_th, simu_counter_right_th;

/** Use mex. */
int simu_mex;

/** Initialise simulation. */
static void
simu_init (void)
{
    int argc;
    char **argv;
    host_get_program_arguments (&argc, &argv);
    if (argc == 2 && strcmp (argv[0], "-m") == 0)
      {
	simu_mex = 1;
	mex_node_connect ();
	argc--; argv++;
      }
    if (argc != 1)
      {
	fprintf (stderr, "Syntax: asserv.host [-m] model\n");
	exit (1);
      }
    simu_robot = models_get (argv[0]);
    if (!simu_robot)
      {
	fprintf (stderr, "unknown model name: %s\n", argv[0]);
	exit (1);
      }
    models_init (simu_robot, &simu_left_model, &simu_right_model,
		 simu_aux_model);
    simu_pos_x = simu_pos_y = simu_pos_a = 0;
}

/** Update simulation position. */
static void
simu_pos_update (double dl, double dr, double footing)
{
    double d = 0.5 * (dl + dr);
    double da = (dr - dl) / footing;
    double na = simu_pos_a + da;
    if (da < 0.0001 && da > -0.0001)
      {
	/* Avoid a division by zero when angle is too small. */
	double a = simu_pos_a + da * 0.5;
	simu_pos_x += d * cos (a);
	simu_pos_y += d * sin (a);
      }
    else
      {
	/* Radius of turn is d / da. */
	simu_pos_x += (sin (na) - sin (simu_pos_a)) * d / da;
	simu_pos_y += (cos (simu_pos_a) - cos (na)) * d / da;
      }
    simu_pos_a = na;
}

/** Update sensors. */
static void
simu_sensor_update (void)
{
    /** Micro-switch sensors. */
    static const double sensors[][2] =
      {
	  { -70.0, 200.0 },
	  { -70.0, -200.0 },
	  { 170.0, 0.0 },
      };
    static const uint8_t sensors_bit[] =
      { _BV (0), _BV (1), _BV (3), };
    static const double table_width = 3000.0, table_height = 2100.0;
    PINC = 0;
    unsigned int i;
    double x, y;
    for (i = 0; i < UTILS_COUNT (sensors); i++)
      {
	/* Compute absolute position. */
	x = simu_pos_x + cos (simu_pos_a) * sensors[i][0]
	    - sin (simu_pos_a) * sensors[i][1];
	y = simu_pos_y + sin (simu_pos_a) * sensors[i][0]
	    + cos (simu_pos_a) * sensors[i][1];
	if (x >= 0.0 && x < table_width && y >= 0.0 && y < table_height)
	    PINC |= sensors_bit[i];
      }
    /** Top zero sensor. */
    double aa;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	aa = simu_aux_model[i].th / simu_aux_model[i].m.i_G * 3;
	if (!(cos (aa) > 0 && fabs (sin (aa)) * 80.0 < 7.5))
	    *aux[i].zero_pin |= aux[i].zero_bv;
      }
}

/** Do a simulation step. */
static void
simu_step (void)
{
    int i;
    double old_left_th, old_right_th, old_aux_th[AC_ASSERV_AUX_NB];
    /* Convert pwm value into voltage. */
    simu_left_model.u = simu_left_model.m.u_max
	* ((double) pwm_left.cur / (PWM_MAX + 1));
    simu_right_model.u = simu_right_model.m.u_max
	* ((double) pwm_right.cur / (PWM_MAX + 1));
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	simu_aux_model[i].u = simu_aux_model[i].m.u_max
	    * ((double) pwm_aux[i].cur / (PWM_MAX + 1));
    /* Make one step. */
    old_left_th = simu_left_model.th;
    old_right_th = simu_right_model.th;
    motor_model_step (&simu_left_model);
    motor_model_step (&simu_right_model);
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	old_aux_th[i] = simu_aux_model[i].th;
	if (simu_robot->aux_motor[i])
	    motor_model_step (&simu_aux_model[i]);
      }
    /* Modify counters. */
    uint32_t counter_left_new;
    uint32_t counter_right_new;
    if (!simu_robot->encoder_separated)
      {
	counter_left_new = simu_left_model.th / (2*M_PI)
	    * simu_robot->main_encoder_steps;
	counter_right_new = simu_right_model.th / (2*M_PI)
	    * simu_robot->main_encoder_steps;
      }
    else
      {
	/* Thanks Thalès. */
	double left_diff = (simu_left_model.th - old_left_th)
	    / simu_left_model.m.i_G * simu_robot->wheel_r;
	double right_diff = (simu_right_model.th - old_right_th)
	    / simu_right_model.m.i_G * simu_robot->wheel_r;
	double sum = left_diff + right_diff;
	double diff = (left_diff - right_diff)
	    * (simu_robot->encoder_footing / simu_robot->footing);
	double left_enc_diff = 0.5 * (sum + diff);
	double right_enc_diff = 0.5 * (sum - diff);
	simu_counter_left_th += left_enc_diff / simu_robot->encoder_wheel_r;
	simu_counter_right_th += right_enc_diff / simu_robot->encoder_wheel_r;
	counter_left_new = simu_counter_left_th / (2*M_PI)
	    * simu_robot->main_encoder_steps;
	counter_right_new = simu_counter_right_th / (2*M_PI)
	    * simu_robot->main_encoder_steps;
      }
    /* Update an integer counter. */
    counter_left_diff = counter_left_new - simu_counter_left;
    counter_left += counter_left_diff;
    simu_counter_left = counter_left_new;
    counter_right_diff = counter_right_new - simu_counter_right;
    counter_right_raw += counter_right_diff;
    uint16_t right_new = fixed_mul_f824 (counter_right_raw,
					 counter_right_correction);
    counter_right_diff = (int16_t) (right_new - counter_right);
    counter_right = right_new;
    simu_counter_right = counter_right_new;
    /* Update auxiliary counter. */
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	if (simu_robot->aux_motor[i])
	  {
	    uint32_t counter_aux_new = simu_aux_model[i].th / (2*M_PI)
		* simu_robot->aux_encoder_steps[i];
	    counter_aux_diff[i] = counter_aux_new - simu_counter_aux[i];
	    counter_aux[i] += counter_aux_diff[i];
	    simu_counter_aux[i] = counter_aux_new;
	  }
      }
    /* Update position. */
    simu_pos_update ((simu_left_model.th - old_left_th)
		     / simu_left_model.m.i_G * simu_robot->wheel_r * 1000,
		     (simu_right_model.th - old_right_th)
		     / simu_right_model.m.i_G * simu_robot->wheel_r * 1000,
		     simu_robot->footing * 1000);
    /* Update sensors. */
    simu_sensor_update ();
}

/** Send information to the other nodes. */
static void
simu_send (void)
{
    int i;
    mex_msg_t *m;
    /* Send position. */
    m = mex_msg_new (0xa0);
    mex_msg_push (m, "hhl", (int16_t) simu_pos_x, (int16_t) simu_pos_y,
		  (int32_t) (1024.0 * simu_pos_a));
    mex_node_send (m);
    /* Send PWM. */
    m = mex_msg_new (0xa1);
    mex_msg_push (m, "hh", pwm_left.cur, pwm_right.cur);
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	mex_msg_push (m, "h", pwm_aux[i].cur);
    mex_node_send (m);
    /* Send Aux position. */
    m = mex_msg_new (0xa8);
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	mex_msg_push (m, "l", (int32_t) (1024.0 * simu_aux_model[i].th
					 / simu_aux_model[i].m.i_G));
    mex_node_send (m);
}

/** Initialise the timer. */
void
timer_init (void)
{
    simu_init ();
}

/** Wait for timer overflow. */
void
timer_wait (void)
{
    if (simu_mex)
	mex_node_wait_date (mex_node_date () + 4);
    simu_step ();
    if (simu_mex)
	simu_send ();
}

/** Read timer value. Used for performance analysis. */
uint8_t
timer_read (void)
{
    return 0;
}

/** Initialize the counters. */
void
counter_init (void)
{
}

/** Update overall counter values and compute diffs. */
void
counter_update (void)
{
}

/** Initialise PWM generator. */
void
pwm_init (void)
{
}

/** Update the hardware PWM values. */
void
pwm_update (void)
{
}

void
eeprom_read_params (void)
{
}

void
eeprom_write_params (void)
{
}

void
eeprom_clear_params (void)
{
}

void
pwm_set_reverse (uint8_t reverse)
{
    pwm_reverse = reverse;
}

