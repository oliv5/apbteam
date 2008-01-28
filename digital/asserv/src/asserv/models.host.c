/* models.host.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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
#include "motor_model.host.h"
#include "models.host.h"

#include <math.h>
#include <string.h>

/* Gloubi model. */
static const struct motor_t gloubi_model =
{
    /* Motor caracteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.18,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    /* Gearbox caracteristics. */
    10,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load caracteristics. */
    4 * 0.02 * 0.02,	/* Load (kg.m^2). */
    /* Wheel caracteristics. */
    0.02,		/* Wheel radius (m). */
    /* Simulation parameters. */
    4.444444 / 1000,	/* Simulation time step (s). */
    1000,		/* Simulation time step division. */
    /* Simulation current state. */
    0,			/* Current time (not realy used) (s). */
    0,			/* Current input voltage (V). */
    0,			/* Current current (A). */
    0,			/* Current angular speed (o for omega) (rad/s). */
    0			/* Current theta (th for theta) (rad). */
};

static const struct robot_t gloubi_robot =
{
    &gloubi_model,
    26.0,		/* Distance between the wheels (m). */
};

/* Taz model. */
static const struct motor_t taz_model =
{
    /* Motor caracteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.18,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    /* Gearbox caracteristics. */
    10,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load caracteristics. */
    10 * 0.04 * 0.04,	/* Load (kg.m^2). */
    /* Wheel caracteristics. */
    0.04,		/* Wheel radius (m). */
    /* Simulation parameters. */
    4.444444 / 1000,	/* Simulation time step (s). */
    1000,		/* Simulation time step division. */
    /* Simulation current state. */
    0,			/* Current time (not realy used) (s). */
    0,			/* Current input voltage (V). */
    0,			/* Current current (A). */
    0,			/* Current angular speed (o for omega) (rad/s). */
    0			/* Current theta (th for theta) (rad). */
};

static const struct robot_t taz_robot =
{
    &taz_model,
    30.0,		/* Distance between the wheels (m). */
};

/* Taz model, with a RE25G and a 1:20.25 ratio gearbox. */
static const struct motor_t tazg_model =
{
    /* Motor caracteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    40.2 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.32,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    /* Gearbox caracteristics. */
    20.25,		/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load caracteristics. */
    10 * 0.04 * 0.04,	/* Load (kg.m^2). */
    /* Wheel caracteristics. */
    0.04,		/* Wheel radius (m). */
    /* Simulation parameters. */
    4.444444 / 1000,	/* Simulation time step (s). */
    1000,		/* Simulation time step division. */
    /* Simulation current state. */
    0,			/* Current time (not realy used) (s). */
    0,			/* Current input voltage (V). */
    0,			/* Current current (A). */
    0,			/* Current angular speed (o for omega) (rad/s). */
    0			/* Current theta (th for theta) (rad). */
};

static const struct robot_t tazg_robot =
{
    &tazg_model,
    30.0,		/* Distance between the wheels (m). */
};

/* Table of models. */
static const struct
{
    const char *name;
    const struct robot_t *robot;
} models[] = {
      { "gloubi", &gloubi_robot },
      { "taz", &taz_robot },
      { "tazg", &tazg_robot },
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

