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
#define _GNU_SOURCE 1 /* Need ISO C99 features as well. */
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

#include "cs.h"
#include "aux.h"

#include "contacts.h"

#include "models.host.h"

/** Simulate some AVR regs. */
uint8_t PORTB, PORTC, PORTD, PINC;

/* Robot model. */
const struct robot_t *simu_robot;

/** Motor models. */
motor_model_t simu_aux_model[AC_ASSERV_AUX_NB];

/** Full encoder values. */
uint32_t simu_encoder_aux[AC_ASSERV_AUX_NB];

/** Use mex. */
int simu_mex;

/** Mex message types. */
uint8_t simu_mex_aux;
uint8_t simu_mex_limits;

/** Counter to limit the interval between information is sent. */
int simu_send_cpt;

static void
simu_handle_limits__update_limit (double *th_limit, int32_t limit_int,
				  double i_G, int sign)
{
    if (limit_int == -1)
	/* No change. */
	;
    else if (limit_int == -2)
	/* INFINITY. */
	*th_limit = sign > 0 ? INFINITY : -INFINITY;
    else
	*th_limit = (double) limit_int / 1024.0 * i_G;
}

static void
simu_handle_limits (void *user, mex_msg_t *msg)
{
    int i;
    int32_t limit_min_int, limit_max_int;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	mex_msg_pop (msg, "ll", &limit_min_int, &limit_max_int);
	simu_handle_limits__update_limit (&simu_aux_model[i].m.th_min,
					  limit_min_int,
					  simu_aux_model[i].m.i_G, -1);
	simu_handle_limits__update_limit (&simu_aux_model[i].m.th_max,
					  limit_max_int,
					  simu_aux_model[i].m.i_G, +1);
      }
}

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
	mex_instance = host_get_instance ("mimot0", 0);
	simu_mex_aux = mex_node_reservef ("%s:aux", mex_instance);
	simu_mex_limits = mex_node_reservef ("%s:limits", mex_instance);
	mex_node_register (simu_mex_limits, simu_handle_limits, 0);
      }
    if (argc != 1)
      {
	fprintf (stderr, "Syntax: dirty.host [-m[interval]] model\n");
	exit (1);
      }
    simu_robot = models_get (argv[0]);
    if (!simu_robot)
      {
	fprintf (stderr, "unknown model name: %s\n", argv[0]);
	exit (1);
      }
    models_init (simu_robot, simu_aux_model);
}

/** Update sensors for Marcel. */
void
simu_sensor_update_marcel (void)
{
}

/** Update sensors for Robospierre. */
void
simu_sensor_update_robospierre (void)
{
    PINC = 0xf0;
    if (simu_aux_model[0].th < 120.0 * 5.0 / 6.0 * simu_aux_model[0].m.i_G)
	PINC |= IO_BV (CONTACT_AUX0_ZERO_IO);
}

/** Do a simulation step. */
static void
simu_step (void)
{
    int i;
    double old_aux_th[AC_ASSERV_AUX_NB];
    /* Convert pwm value into voltage. */
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	simu_aux_model[i].u = simu_robot->u_max
	    * ((double) output_aux[i].cur / (OUTPUT_MAX + 1));
    /* Make one step. */
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	old_aux_th[i] = simu_aux_model[i].th;
	if (simu_robot->aux_motor[i])
	    motor_model_step (&simu_aux_model[i]);
      }
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

/** Wait for timer overflow. */
void
timer_wait (void)
{
    if (simu_mex)
	mex_node_wait_date (mex_node_date () + 4);
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

