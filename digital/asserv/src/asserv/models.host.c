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
#include "common.h"

#include "motor_model.host.h"
#include "models.host.h"
#include "simu.host.h"

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
    0.0, 0.0, { NULL, NULL }, { 0, 0 }, NULL
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
    0.0, 0.0, { NULL, NULL }, { 0, 0 }, NULL
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
    0.0, 0.0, { NULL, NULL }, { 0, 0 }, NULL
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
    24.0,		/* Maximum voltage (V). */
    /* WARNING: Giboulée arm use a 12V motor on 24V power, PWM should be
     * limited to half scale. */
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
    2500,
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
    /** Auxiliary motors, NULL if not present. */
    { &giboulee_arm_model, NULL },
    /** Number of steps for each auxiliary motor encoder. */
    { 500, 0 },
    /** Sensor update function. */
    simu_sensor_update_giboulee,
};

/* AquaJim arm model, with a RE40G and a 1:4 + 15:80 ratio gearbox. */
static const struct motor_def_t aquajim_arm_model =
{
    /* Motor characteristics. */
    317 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    30.2 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    0.316,		/* Terminal resistance (Ohm). */
    0.08 / 1000,	/* Terminal inductance (H). */
    24.0,		/* Maximum voltage (V). */
    /* Gearbox characteristics. */
    4.0 * 80.0 / 15.0,	/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.05 * 2.5 * 0.06 * 0.06,	/* Load (kg.m^2). */
};

/* AquaJim elevator model, with a RE25CLL and a 1:10 ratio gearbox. */
static const struct motor_def_t aquajim_elevator_model =
{
    /* Motor characteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.18,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    24.0,		/* Maximum voltage (V). */
    /* WARNING: this motor uses a 12V motor on 24V power, PWM should be
     * limited to half scale. */
    /* Gearbox characteristics. */
    10,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.200 * 0.01 * 0.01,/* Load (kg.m^2). */
    /* This is a pifometric estimation. */
};

/* AquaJim, APBTeam 2009. */
static const struct robot_t aquajim_robot =
{
    /* Main motors. */
    &amax32ghp_model,
    /* Number of steps on the main motors encoders. */
    2500,
    /* Wheel radius (m). */
    0.065 / 2,
    /* Distance between the wheels (m). */
    0.16,
    /* Weight of the robot (kg). */
    20.0,
    /* Distance of the gravity center from the center of motors axis (m). */
    0.0,
    /* Whether the encoder is mounted on the main motor (false) or not (true). */
    1,
    /** Encoder wheel radius (m). */
    0.063 / 2,
    /** Distance between the encoders wheels (m). */
    0.28,
    /** Auxiliary motors, NULL if not present. */
    { &aquajim_arm_model, &aquajim_elevator_model },
    /** Number of steps for each auxiliary motor encoder. */
    { 250, 250 },
    /** Sensor update function. */
    simu_sensor_update_aquajim,
};

/* Marcel elevator model, with a Faulhaber 2657 and a 1:9.7 ratio gearbox. */
static const struct motor_def_t marcel_elevator_model =
{
    /* Motor characteristics. */
    274 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    34.8 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.84,		/* Terminal resistance (Ohm). */
    0.380 / 1000,	/* Terminal inductance (H). */
    24.0,		/* Maximum voltage (V). */
    /* Gearbox characteristics. */
    9.7,		/* Gearbox ratio. */
    0.80,		/* Gearbox efficiency. */
    /* Load characteristics. */
    1.0 * 0.01 * 0.01,	/* Load (kg.m^2). */
    /* This is a pifometric estimation. */
};

/* Marcel, APBTeam 2010. */
static const struct robot_t marcel_robot =
{
    /* Main motors. */
    &amax32ghp_model,
    /* Number of steps on the main motors encoders. */
    2500,
    /* Wheel radius (m). */
    0.065 / 2,
    /* Distance between the wheels (m). */
    0.16,
    /* Weight of the robot (kg). */
    10.0,
    /* Distance of the gravity center from the center of motors axis (m). */
    0.0,
    /* Whether the encoder is mounted on the main motor (false) or not (true). */
    1,
    /** Encoder wheel radius (m). */
    0.063 / 2,
    /** Distance between the encoders wheels (m). */
    0.28,
    /** Auxiliary motors, NULL if not present. */
    { &marcel_elevator_model, NULL },
    /** Number of steps for each auxiliary motor encoder. */
    { 512, 0 },
    /** Sensor update function. */
    simu_sensor_update_marcel,
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
      { "aquajim", &aquajim_robot },
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
models_init (const struct robot_t *robot, struct motor_t *main_motor_left,
	     struct motor_t *main_motor_right, struct motor_t aux_motor[])
{
    int i;
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

