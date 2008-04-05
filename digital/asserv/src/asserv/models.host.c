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
#include "motor_model.host.h"
#include "models.host.h"

#include <math.h>
#include <string.h>

/* RE25CLL with 1:10 gearbox model. */
static const struct motor_def_t re25cll_model =
{
    /* Motor characteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.18,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    12.0,		/* Maximum voltage (V). */
    /* Gearbox characteristics. */
    10,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
};

/* RE25G with 1:20.25 gearbox model. */
static const struct motor_def_t re25g_model =
{
    /* Motor characteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.32,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    24.0,		/* Maximum voltage (V). */
    /* Gearbox characteristics. */
    20.25,		/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
};

/* AMAX32GHP with 1:16 gearbox model. */
static const struct motor_def_t amax32ghp_model =
{
    /* Motor characteristics. */
    269 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    25.44 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    3.99,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    24.0,		/* Maximum voltage (V). */
    /* Gearbox characteristics. */
    16,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
};

/* Gloubi, Efrei 2006. */
static const struct robot_t gloubi_robot =
{
    /* Main motors. */
    &re25cll_model,
    /* Number of steps on the main motors encoders. */
    500,
    /* Wheel radius (m). */
    0.02,
    /* Distance between the wheels (m). */
    0.26,
    /* Weight of the robot (kg). */
    4.0,
    /* Distance of the gravity center from the center of motors axis (m). */
    13.0, // approx
    /* Whether the encoder is mounted on the main motor (false) or not (true). */
    0,
    0.0, 0.0, NULL, 0
};

/* Taz, APBTeam/Efrei 2005. */
static const struct robot_t taz_robot =
{
    /* Main motors. */
    &re25cll_model,
    /* Number of steps on the main motors encoders. */
    500,
    /* Wheel radius (m). */
    0.04,
    /* Distance between the wheels (m). */
    0.30,
    /* Weight of the robot (kg). */
    10.0,
    /* Distance of the gravity center from the center of motors axis (m). */
    0.0,
    /* Whether the encoder is mounted on the main motor (false) or not (true). */
    0,
    0.0, 0.0, NULL, 0
};

/* TazG, Taz with RE25G motors. */
static const struct robot_t tazg_robot =
{
    /* Main motors. */
    &re25g_model,
    /* Number of steps on the main motors encoders. */
    500,
    /* Wheel radius (m). */
    0.04,
    /* Distance between the wheels (m). */
    0.30,
    /* Weight of the robot (kg). */
    10.0,
    /* Distance of the gravity center from the center of motors axis (m). */
    0.0,
    /* Whether the encoder is mounted on the main motor (false) or not (true). */
    0,
    0.0, 0.0, NULL, 0
};

/* Giboulée arm model, with a RE25CLL and a 1:10 ratio gearbox. */
static const struct motor_def_t giboulee_arm_model =
{
    /* Motor characteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.18,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    12.0,		/* Maximum voltage (V). */
    /* Gearbox characteristics. */
    10,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.200 * 0.1 * 0.1,	/* Load (kg.m^2). */
};

/* Giboulée, APBTeam 2008. */
static const struct robot_t giboulee_robot =
{
    /* Main motors. */
    &amax32ghp_model,
    /* Number of steps on the main motors encoders. */
    5000,
    /* Wheel radius (m). */
    0.065 / 2,
    /* Distance between the wheels (m). */
    0.16,
    /* Weight of the robot (kg). */
    10.0,
    /* Distance of the gravity center from the center of motors axis (m). */
    0.10,
    /* Whether the encoder is mounted on the main motor (false) or not (true). */
    1,
    /** Encoder wheel radius (m). */
    0.063 / 2,
    /** Distance between the encoders wheels (m). */
    0.28,
    /** First auxiliary motor or NULL if none. */
    &giboulee_arm_model,
    /** Number of steps on the first auxiliary motor encoder. */
    500,
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
      { "giboulee", &giboulee_robot },
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
models_init (const struct robot_t *robot, struct motor_t *main_motor_left,
	     struct motor_t *main_motor_right, struct motor_t *aux0_motor)
{
    if (main_motor_left)
      {
	main_motor_left->m = *robot->main_motor;
	main_motor_left->m.J = robot->weight * robot->wheel_r
	    * robot->wheel_r / 2;
	main_motor_left->h = ECHANT_PERIOD;
	main_motor_left->d = 1000;
      }
    if (main_motor_right)
      {
	main_motor_right->m = *robot->main_motor;
	main_motor_right->m.J = robot->weight * robot->wheel_r
	    * robot->wheel_r / 2;
	main_motor_right->h = ECHANT_PERIOD;
	main_motor_right->d = 1000;
      }
    if (aux0_motor && robot->aux0_motor)
      {
	aux0_motor->m = *robot->aux0_motor;
	aux0_motor->h = ECHANT_PERIOD;
	aux0_motor->d = 1000;
      }
}

