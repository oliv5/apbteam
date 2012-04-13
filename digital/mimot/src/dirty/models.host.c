/* models.host.c */
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

#include "io.h"

#include "models.host.h"
#include "simu.host.h"

#include "contacts.h"

#include <math.h>
#include <string.h>

static void
simu_sensor_update_robospierre (void);

static void
simu_sensor_update_guybrush (void);

/* Marcel, APBTeam 2010. */
static const struct robot_t marcel_robot =
{
    /** Auxiliary motors, NULL if not present. */
    { &motor_model_def_faulhaber_2342_x3_71,
      &motor_model_def_faulhaber_2342_x3_71 },
    /** Motors voltage (V). */
    24.0,
    /** Number of steps for each auxiliary motor encoder. */
    { 256, 256 },
    /** Load for auxiliary motors (kg.m^2). */
    { 0.100 * 0.005 * 0.005, 0.100 * 0.005 * 0.005 },
    /** Sensor update function. */
    NULL,
    /** Initialisation function. */
    NULL,
};

/* Robospierre, APBTeam 2011. */
static const struct robot_t robospierre_robot =
{
    /** Auxiliary motors, NULL if not present. */
    { &motor_model_def_faulhaber_2342_x3_71, &motor_model_def_amax32ghp_x16 },
    /** Motors voltage (V). */
    24.0,
    /** Number of steps for each auxiliary motor encoder. */
    { 256, 250 },
    /** Load for auxiliary motors (kg.m^2). */
    { 0.100 * 0.005 * 0.005, 0.200 * 0.010 * 0.010 },
    /** Sensor update function. */
    simu_sensor_update_robospierre,
    /** Initialisation function. */
    NULL,
};

/* Guybrush, APBTeam 2012. */
static const struct robot_t guybrush_robot =
{
    /** Auxiliary motors, NULL if not present. */
    { &motor_model_def_amax32ghp_x16, &motor_model_def_amax32ghp_x16 },
    /** Motors voltage (V). */
    24.0,
    /** Number of steps for each auxiliary motor encoder. */
    { 250, 250 },
    /** Load for auxiliary motors (kg.m^2). */
    { 1.000 * 0.05 * 0.05, 0.200 * 0.010 * 0.010 },
    /** Sensor update function. */
    simu_sensor_update_guybrush,
    /** Initialisation function. */
    NULL,
};

/* Table of models. */
static const struct
{
    const char *name;
    const struct robot_t *robot;
} models[] = {
      { "marcel", &marcel_robot },
      { "robospierre", &robospierre_robot },
      { "guybrush", &guybrush_robot },
      { 0, 0 }
};

/** Get a pointer to a model by name, or return 0. */
const struct robot_t *
models_get (const char *name)
{
    int i;
    for (i = 0; models[i].name; i++)
      {
	if (strcmp (models[i].name, name) == 0)
	    return models[i].robot;
      }
    return 0;
}

/** Initialise simulation models. */
void
models_init (const struct robot_t *robot, motor_model_t aux_motor[])
{
    int i;
    if (aux_motor)
      {
	for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	  {
	    if (robot->aux_motor[i])
	      {
		aux_motor[i].m = *robot->aux_motor[i];
		aux_motor[i].m.J = robot->aux_load[i];
		aux_motor[i].h = ECHANT_PERIOD;
		aux_motor[i].d = 1000;
	      }
	  }
      }
    if (robot->init)
	robot->init (robot, aux_motor);
}

/** Update sensors for Robospierre. */
static void
simu_sensor_update_robospierre (void)
{
    PINC = 0xf0;
    if (simu_aux_model[0].th < 120.0 * 5.0 / 6.0 * simu_aux_model[0].m.i_G)
	PINC |= IO_BV (CONTACT_AUX0_ZERO_IO);
}

/** Update sensors for Guybrush. */
static void
simu_sensor_update_guybrush (void)
{
    PINC = 0xf0;
    double rem = fmod (simu_aux_model[0].th / simu_aux_model[0].m.i_G,
		       2 * M_PI);
    if (rem < 0.0)
	rem += 2 * M_PI;
    if (rem > 0.01)
	PINC |= IO_BV (CONTACT_AUX0_ZERO_IO);
}

