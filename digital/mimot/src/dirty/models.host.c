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

#include "motor_model.host.h"
#include "models.host.h"
#include "simu.host.h"

#include <math.h>
#include <string.h>

/* Marcel clamp, with a Faulhaber 2342 and 23/1 3.71:1 gearbox model. */
static const struct motor_def_t marcel_clamp_f2342_model =
{
    /* Motor characteristics. */
    366 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    26.10 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    7.10,		/* Terminal resistance (Ohm). */
    0.265 / 1000,	/* Terminal inductance (H). */
    24.0,		/* Maximum voltage (V). */
    /* Gearbox characteristics. */
    3.71,		/* Gearbox ratio. */
    0.88,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.100 * 0.005 * 0.005,/* Load (kg.m^2). */
    /* This is a pifometric estimation. */
    /* Hardware limits. */
    0.0, +INFINITY,
};

/* Marcel, APBTeam 2010. */
static const struct robot_t marcel_robot =
{
    /** Auxiliary motors, NULL if not present. */
    { &marcel_clamp_f2342_model, &marcel_clamp_f2342_model },
    /** Number of steps for each auxiliary motor encoder. */
    { 512, 512 },
    /** Sensor update function. */
    simu_sensor_update_marcel,
};

/* Table of models. */
static const struct
{
    const char *name;
    const struct robot_t *robot;
} models[] = {
      { "marcel", &marcel_robot },
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
models_init (const struct robot_t *robot, struct motor_t aux_motor[])
{
    int i;
    if (aux_motor)
      {
	for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	  {
	    if (robot->aux_motor[i])
	      {
		aux_motor[i].m = *robot->aux_motor[i];
		aux_motor[i].h = ECHANT_PERIOD;
		aux_motor[i].d = 1000;
	      }
	  }
      }
}

