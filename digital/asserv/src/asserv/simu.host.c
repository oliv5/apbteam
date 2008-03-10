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
#include "modules/utils/utils.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "pwm.h"

#include "motor_model.host.h"
#include "models.host.h"

/** Simulate some AVR regs. */
uint8_t DDRF, PORTC, PORTD, PORTE, PORTF, PORTG, PINC;

/** Overall counter values. */
uint16_t counter_left, counter_right;
/** Counter differences since last update.
 * Maximum of 9 significant bits, sign included. */
int16_t counter_left_diff, counter_right_diff;

/** PWM values, this is an error if absolute value is greater than the
 * maximum. */
int16_t pwm_left, pwm_right, pwm_aux0;
/** PWM reverse directions. */
uint8_t pwm_reverse;

struct motor_t simu_left_model, simu_right_model;

/** Computed simulated position (mm). */
double simu_pos_x, simu_pos_y, simu_pos_a;

/** Distance between wheels. */
double simu_footing;

/** Initialise simulation. */
static void
simu_init (void)
{
    int argc;
    char **argv;
    const struct robot_t *m;
    host_get_program_arguments (&argc, &argv);
    if (argc != 1)
      {
	fprintf (stderr, "need model name as first argument\n");
	exit (1);
      }
    m = models_get (argv[0]);
    if (!m)
      {
	fprintf (stderr, "unknown model name: %s\n", argv[0]);
	exit (1);
      }
    simu_left_model = *m->motor;
    simu_right_model = *m->motor;
    simu_footing = m->footing;
    simu_pos_x = simu_pos_y = 0;
}

/** Update simulation position. */
static void
simu_pos_update (double dl, double dr)
{
    double d = 0.5 * (dl + dr);
    double da = (dr - dl) / simu_footing;
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

/** Do a simulation step. */
static void
simu_step (void)
{
    double old_left_th, old_right_th;
    /* Convert pwm value into voltage. */
    assert (pwm_left >= -PWM_MAX && pwm_left <= PWM_MAX);
    assert (pwm_right >= -PWM_MAX && pwm_right <= PWM_MAX);
    simu_left_model.u = (double) (pwm_left + 1) / (PWM_MAX + 1);
    simu_right_model.u = (double) (pwm_right + 1) / (PWM_MAX + 1);
    /* Make one step. */
    old_left_th = simu_left_model.th;
    old_right_th = simu_right_model.th;
    motor_model_step (&simu_left_model);
    motor_model_step (&simu_right_model);
    /* Modify counters. */
    counter_left_diff = (simu_left_model.th - old_left_th) / (2*M_PI)
        * 500 * simu_left_model.i_G;
    counter_left += counter_left_diff;
    counter_right_diff = (simu_right_model.th - old_right_th) / (2*M_PI)
        * 500 * simu_right_model.i_G;
    counter_right += counter_right_diff;
    /* Update position */
    simu_pos_update ((simu_left_model.th - old_left_th)
		     * simu_left_model.i_G * simu_left_model.w_r * 1000,
		     (simu_right_model.th - old_right_th)
		     * simu_right_model.i_G * simu_right_model.w_r * 1000);
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
    simu_step ();
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

