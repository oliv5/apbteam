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
#include "modules/math/fixed/fixed.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "cs.h"

#include "models.host.h"

/** Simulate some AVR regs. */
uint8_t DDRF, PORTB, PORTC, PORTD, PORTE, PORTF, PORTG, PINC;

/** Index of loaded eeprom block. */
int8_t eeprom_loaded = -1;

/* Robot model. */
const struct robot_t *simu_robot;

/** Motor models. */
motor_model_t simu_left_model, simu_right_model,
	      simu_aux_model[AC_ASSERV_AUX_NB];

/** Computed simulated position (mm, rad). */
double simu_pos_x, simu_pos_y, simu_pos_a;

/** Full encoder values. */
uint32_t simu_encoder_left, simu_encoder_right,
	 simu_encoder_aux[AC_ASSERV_AUX_NB];
double simu_encoder_left_th, simu_encoder_right_th;

/** Use mex. */
int simu_mex;

/** Do not sleep. */
int simu_fast;

/** Mex message types. */
uint8_t simu_mex_position;
uint8_t simu_mex_pwm;
uint8_t simu_mex_aux;

/** Counter to limit the interval between information is sent. */
int simu_send_cpt;

/** Initialise simulation. */
static void
simu_init (void)
{
    int argc;
    char **argv;
    const char *mex_instance;
    host_get_program_arguments (&argc, &argv);
    if (argc == 2 && strncmp (argv[0], "-m", 2) == 0)
      {
	simu_mex = atoi (argv[0] + 2);
	argc--; argv++;
	if (!simu_mex) simu_mex = 1;
	simu_send_cpt = simu_mex;
	mex_node_connect ();
	mex_instance = host_get_instance ("asserv0", 0);
	simu_mex_position = mex_node_reservef ("%s:position", mex_instance);
	simu_mex_pwm = mex_node_reservef ("%s:pwm", mex_instance);
	simu_mex_aux = mex_node_reservef ("%s:aux", mex_instance);
      }
    simu_fast = simu_mex;
    if (argc != 1)
      {
	fprintf (stderr, "Syntax: asserv.host [-m[interval]] model\n");
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

/** Compute a robot point absolute position. */
void
simu_compute_absolute_position (double p_x, double p_y, double *x, double *y)
{
    double c = cos (simu_pos_a);
    double s = sin (simu_pos_a);
    *x = simu_pos_x + c * p_x - s * p_y;
    *y = simu_pos_y + s * p_x + c * p_y;
}

/** Do a simulation step. */
static void
simu_step (void)
{
    int i;
    double old_left_th, old_right_th, old_aux_th[AC_ASSERV_AUX_NB];
    /* Convert pwm value into voltage. */
    simu_left_model.u = simu_robot->u_max
	* ((double) output_left.cur / (OUTPUT_MAX + 1));
    simu_right_model.u = simu_robot->u_max
	* ((double) output_right.cur / (OUTPUT_MAX + 1));
#if AC_ASSERV_AUX_NB
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	simu_aux_model[i].u = simu_robot->u_max
	    * ((double) output_aux[i].cur / (OUTPUT_MAX + 1));
#endif
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
    /* Update position. */
    double old_pos_x = simu_pos_x, old_pos_y = simu_pos_y,
	   old_pos_a = simu_pos_a;
    simu_pos_update ((simu_left_model.th - old_left_th)
		     / simu_left_model.m.i_G * simu_robot->wheel_r * 1000,
		     (simu_right_model.th - old_right_th)
		     / simu_right_model.m.i_G * simu_robot->wheel_r * 1000,
		     simu_robot->footing * 1000);
    /* Check robot is still on the table. */
    if (simu_robot->table_test)
      {
	static int old_out = 1;
	int out = 0;
	for (i = 0; i < CORNERS_NB; i++)
	  {
	    if (!simu_robot->table_test (simu_robot->corners[i][0],
					 simu_robot->corners[i][1]))
		out = 1;
	  }
	/* If out, cancel movement. */
	if (out && !old_out)
	  {
	    simu_pos_x = old_pos_x;
	    simu_pos_y = old_pos_y;
	    simu_pos_a = old_pos_a;
	    simu_left_model.th = old_left_th;
	    simu_right_model.th = old_right_th;
	  }
	else
	  {
	    old_out = out;
	  }
      }
    /* Modify encoders. */
    uint32_t encoder_left_new;
    uint32_t encoder_right_new;
    if (!simu_robot->encoder_separated)
      {
	encoder_left_new = simu_left_model.th / (2*M_PI)
	    * simu_robot->main_encoder_steps;
	encoder_right_new = simu_right_model.th / (2*M_PI)
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
	simu_encoder_left_th += left_enc_diff / simu_robot->encoder_wheel_r;
	simu_encoder_right_th += right_enc_diff / simu_robot->encoder_wheel_r;
	encoder_left_new = simu_encoder_left_th / (2*M_PI)
	    * simu_robot->main_encoder_steps;
	encoder_right_new = simu_encoder_right_th / (2*M_PI)
	    * simu_robot->main_encoder_steps;
      }
    /* Update an integer encoder. */
    encoder_left.diff = encoder_left_new - simu_encoder_left;
    encoder_left.cur += encoder_left.diff;
    simu_encoder_left = encoder_left_new;
    encoder_right.diff = encoder_right_new - simu_encoder_right;
    encoder_right.cur += encoder_right.diff;
    simu_encoder_right = encoder_right_new;
#if AC_ASSERV_AUX_NB
    /* Update auxiliary encoder. */
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	if (simu_robot->aux_motor[i])
	  {
	    uint32_t encoder_aux_new = simu_aux_model[i].th / (2*M_PI)
		* simu_robot->aux_encoder_steps[i];
	    encoder_aux[i].diff = encoder_aux_new - simu_encoder_aux[i];
	    encoder_aux[i].cur += encoder_aux[i].diff;
	    simu_encoder_aux[i] = encoder_aux_new;
	  }
	else
	  {
	    encoder_aux[i].diff = 0;
	    encoder_aux[i].cur = 0;
	    simu_encoder_aux[i] = 0;
	  }
      }
#endif
    /* Update sensors. */
    if (simu_robot->sensor_update)
	simu_robot->sensor_update ();
}

/** Send information to the other nodes. */
static void
simu_send (void)
{
    static int first = 1;
    int i;
    mex_msg_t *m;
    /* Send position. */
    static int16_t simu_pos_x_sent, simu_pos_y_sent;
    static int32_t simu_pos_a_sent;
    int16_t simu_pos_x_to_send = simu_pos_x;
    int16_t simu_pos_y_to_send = simu_pos_y;
    int32_t simu_pos_a_to_send = 1024.0 * simu_pos_a;
    if (first
	|| simu_pos_x_to_send != simu_pos_x_sent
	|| simu_pos_y_to_send != simu_pos_y_sent
	|| simu_pos_a_to_send != simu_pos_a_sent)
      {
	m = mex_msg_new (simu_mex_position);
	mex_msg_push (m, "hhl", simu_pos_x_to_send, simu_pos_y_to_send,
		      simu_pos_a_to_send);
	mex_node_send (m);
	simu_pos_x_sent = simu_pos_x_to_send;
	simu_pos_y_sent = simu_pos_y_to_send;
	simu_pos_a_sent = simu_pos_a_to_send;
      }
    /* Send PWM. */
    static int16_t output_left_sent, output_right_sent;
    if (first
	|| output_left_sent == output_left.cur
	|| output_right_sent == output_right.cur) // BUG?
      {
	m = mex_msg_new (simu_mex_pwm);
	mex_msg_push (m, "hh", output_left.cur, output_right.cur);
#if AC_ASSERV_AUX_NB
	for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	    mex_msg_push (m, "h", output_aux[i].cur);
#endif
	mex_node_send (m);
	output_left_sent = output_left.cur;
	output_right_sent = output_right.cur;
      }
    /* Send Aux position. */
    static int32_t simu_aux_model_sent[AC_ASSERV_AUX_NB];
    int32_t simu_aux_model_to_send[AC_ASSERV_AUX_NB];
    int simu_aux_model_changed = 0;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	simu_aux_model_to_send[i] = 1024.0 * simu_aux_model[i].th
	    / simu_aux_model[i].m.i_G;
	if (!first && simu_aux_model_to_send[i] != simu_aux_model_sent[i])
	    simu_aux_model_changed = 1;
      }
    if (first || simu_aux_model_changed)
      {
	m = mex_msg_new (simu_mex_aux);
	for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	  {
	    if (simu_robot->aux_motor[i])
		mex_msg_push (m, "l", simu_aux_model_to_send[i]);
	    else
		mex_msg_push (m, "l", 0);
	    simu_aux_model_sent[i] = simu_aux_model_to_send[i];
	  }
	mex_node_send (m);
      }
    /* First send done. */
    first = 0;
}

/** Initialise the timer. */
void
timer_init (void)
{
    simu_init ();
}

/** Slow down program execution. */
void
simu_wait (int freq)
{
#define ONE_SEC_NS 1000000000ll
    struct timeval tv;
    static long long int last_ns = 0;
    long long int now_ns;
    gettimeofday (&tv, NULL);
    now_ns = tv.tv_sec * ONE_SEC_NS + tv.tv_usec * 1000;
    if (last_ns == 0 || now_ns - last_ns > ONE_SEC_NS)
	last_ns = now_ns;
    last_ns = last_ns + ONE_SEC_NS / freq;
    long long int diff_ns = last_ns - now_ns;
    if (diff_ns > 0)
      {
	struct timespec ts;
	ts.tv_sec = diff_ns / ONE_SEC_NS;
	ts.tv_nsec = diff_ns % ONE_SEC_NS;
	nanosleep (&ts, &ts);
      }
}

/** Wait for timer overflow. */
void
timer_wait (void)
{
    if (simu_mex)
	mex_node_wait_date (mex_node_date () + 4);
    if (!simu_fast)
	simu_wait (225);
    simu_step ();
    if (simu_mex && !--simu_send_cpt)
      {
	simu_send_cpt = simu_mex;
	simu_send ();
      }
}

/** Read timer value. Used for performance analysis. */
uint8_t
timer_read (void)
{
    return 0;
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

