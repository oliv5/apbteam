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

#include "modules/utils/utils.h"

#include "io.h"

#include "models.host.h"
#include "simu.host.h"

#include "aux_traj.h"

#include AC_ASSERV_CONTACTS_H

#include <math.h>
#include <string.h>

#define NO_CORNER { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }

static void
simu_sensor_update_giboulee (void);

static void
simu_sensor_update_aquajim (void);

static void
simu_sensor_update_marcel (void);

static void
marcel_robot_init (const struct robot_t *robot, struct motor_model_t *main_motor_left,
		   struct motor_model_t *main_motor_right, struct motor_model_t aux_motor[]);

static int
simu_table_test_robospierre (double p_x, double p_y);

/* Gloubi, Efrei 2006. */
static const struct robot_t gloubi_robot =
{
    /* Main motors. */
    &motor_model_def_re25cll_x10,
    /* Motors voltage (V). */
    12.0,
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
    0.0, 0.0, { NULL, NULL }, { 0, 0 }, { 0, 0 }, NULL, NULL, NO_CORNER, NULL
};

/* Taz, APBTeam/Efrei 2005. */
static const struct robot_t taz_robot =
{
    /* Main motors. */
    &motor_model_def_re25cll_x10,
    /* Motors voltage (V). */
    12.0,
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
    0.0, 0.0, { NULL, NULL }, { 0, 0 }, { 0, 0 }, NULL, NULL, NO_CORNER, NULL
};

/* TazG, Taz with RE25G motors. */
static const struct robot_t tazg_robot =
{
    /* Main motors. */
    &motor_model_def_re25g_x20_25,
    /* Motors voltage (V). */
    24.0,
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
    0.0, 0.0, { NULL, NULL }, { 0, 0 }, { 0, 0 }, NULL, NULL, NO_CORNER, NULL
};

/* Giboulée, APBTeam 2008. */
static const struct robot_t giboulee_robot =
{
    /* Main motors. */
    &motor_model_def_amax32ghp_x16,
    /* Motors voltage (V). */
    24.0,
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
    { &motor_model_def_re25cll_x10, NULL },
    /** Number of steps for each auxiliary motor encoder. */
    { 500, 0 },
    /** Load for auxiliary motors (kg.m^2). */
    { 0.200 * 0.1 * 0.1, 0 },
    /** Sensor update function. */
    simu_sensor_update_giboulee,
    NULL, NO_CORNER, NULL
};

/* AquaJim arm model, with a RE40G and a 1:4 + 15:80 ratio gearbox. */
static motor_model_def_t aquajim_arm_model_def =
{
    /* Motor characteristics. */
    317 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    30.2 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    0.316,		/* Terminal resistance (Ohm). */
    0.08 / 1000,	/* Terminal inductance (H). */
    /* Gearbox characteristics. */
    4.0 * 80.0 / 15.0,	/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
    /* Hardware limits. */
    -INFINITY, +INFINITY,
};

/* AquaJim, APBTeam 2009. */
static const struct robot_t aquajim_robot =
{
    /* Main motors. */
    &motor_model_def_amax32ghp_x16,
    /* Motors voltage (V). */
    24.0,
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
    { &aquajim_arm_model_def, &motor_model_def_re25cll_x10 },
    /** Number of steps for each auxiliary motor encoder. */
    { 250, 250 },
    /** Load for auxiliary motors (kg.m^2). */
    { 0.05 * 2.5 * 0.06 * 0.06, 0.200 * 0.01 * 0.01 /* Pif */ },
    /** Sensor update function. */
    simu_sensor_update_aquajim,
    NULL, NO_CORNER, NULL
};

/* Marcel, APBTeam 2010. */
static const struct robot_t marcel_robot =
{
    /* Main motors. */
    &motor_model_def_amax32ghp_x16,
    /* Motors voltage (V). */
    24.0,
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
    { &motor_model_def_faulhaber_2657_x9_7, &motor_model_def_re25cll_x10 },
    /** Number of steps for each auxiliary motor encoder. */
    { 256, 250 },
    /** Load for auxiliary motors (kg.m^2). */
    { 1.0 * 0.01115 * 0.01115, 0.100 * 0.01 * 0.01 /* Pif */ },
    /** Sensor update function. */
    simu_sensor_update_marcel,
    NULL, NO_CORNER,
    /** Initialisation function. */
    marcel_robot_init,
};

/* Robospierre, APBTeam 2011. */
static const struct robot_t robospierre_robot =
{
    /* Main motors. */
    &motor_model_def_faulhaber_2657_x9_7,
    /* Motors voltage (V). */
    24.0,
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
    { NULL, NULL },
    /** Number of steps for each auxiliary motor encoder. */
    { 0, 0 },
    /** Load for auxiliary motors (kg.m^2). */
    { 0, 0 },
    /** Sensor update function. */
    NULL,
    /** Table test function, return false if given robot point is not in
     * table. */
    simu_table_test_robospierre,
    /** Robot corners, from front left, then clockwise. */
    { { 150, 110 }, { 150, -110 }, { -150, -110 }, { -150, 110 } },
    /** Initialisation function. */
    NULL,
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
      { "robospierre", &robospierre_robot },
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
models_init (const struct robot_t *robot, motor_model_t *main_motor_left,
	     motor_model_t *main_motor_right, motor_model_t aux_motor[])
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
		aux_motor[i].m.J = robot->aux_load[i];
		aux_motor[i].h = ECHANT_PERIOD;
		aux_motor[i].d = 1000;
	      }
	  }
      }
    if (robot->init)
	robot->init (robot, main_motor_left, main_motor_right, aux_motor);
}

/** Update sensors for Giboulee. */
void
simu_sensor_update_giboulee (void)
{
    /** Micro-switch sensors. */
    static const double sensors[][2] =
      {
	  { -70.0, 200.0 },
	  { -70.0, -200.0 },
	  { 170.0, 0.0 },
      };
    static const uint8_t sensors_bit[] =
      { IO_BV (CONTACT_BACK_LEFT_IO), IO_BV (CONTACT_BACK_RIGHT_IO),
	IO_BV (CONTACT_CENTER_IO), };
    static const double table_width = 3000.0, table_height = 2100.0;
    PINC = 0;
    unsigned int i;
    double x, y;
    for (i = 0; i < UTILS_COUNT (sensors); i++)
      {
	/* Compute absolute position. */
	simu_compute_absolute_position (sensors[i][0], sensors[i][1], &x, &y);
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

/** Update sensors for AquaJim. */
void
simu_sensor_update_aquajim (void)
{
    /** Micro-switch sensors. */
    static const double sensors[][2] =
      {
	  { -150.0, 70.0 },
	  { -150.0, -70.0 },
	  { 150.0, 130.0 },
	  { 150.0, -130.0 },
	  { 150.0, 0.0 },
      };
    static const uint8_t sensors_bit[] =
      { IO_BV (CONTACT_BACK_LEFT_IO), IO_BV (CONTACT_BACK_RIGHT_IO),
	IO_BV (CONTACT_FRONT_LEFT_IO), IO_BV (CONTACT_FRONT_RIGHT_IO),
	IO_BV (CONTACT_CENTER_IO), };
    static const double table_width = 3000.0, table_height = 2100.0;
    static const double center_zone_radius = 150.0;
    PINC = 0;
    unsigned int i;
    double x, y, cx, cy, ds;
    for (i = 0; i < UTILS_COUNT (sensors); i++)
      {
	/* Compute absolute position. */
	simu_compute_absolute_position (sensors[i][0], sensors[i][1], &x, &y);
	cx = table_width / 2 - x;
	cy = table_height / 2 - y;
	ds = cx * cx + cy * cy;
	if (x >= 0.0 && x < table_width && y >= 0.0 && y < table_height
	    && ds > center_zone_radius * center_zone_radius)
	    PINC |= sensors_bit[i];
      }
    /** Top zero sensors. */
    double aa = simu_aux_model[0].th / simu_aux_model[0].m.i_G
	/* Almost open. */
	+ 2 * M_PI / 6 / 10
	/* Turn at the next hole. */
	- 2 * M_PI / 3
	/* Mechanical offset. */
	+ 2 * M_PI * 0x43e / simu_robot->aux_encoder_steps[0] /
	simu_aux_model[0].m.i_G;
    double apos = aa / (2 * M_PI / 3);
    if (apos - floor (apos) > 0.5)
	PINC |= IO_BV (CONTACT_AUX0_ZERO_IO);
    if (simu_aux_model[1].th >= 0)
	PINC |= IO_BV (CONTACT_AUX1_ZERO_IO);
}

/** Update sensors for Marcel. */
void
simu_sensor_update_marcel (void)
{
    /** Micro-switch sensors. */
    static const double sensors[][2] =
      {
	  { -160.0, 90.0 },
	  { -160.0, -90.0 },
	  { 120.0, 155.0 },
	  { 120.0, -155.0 },
      };
    static const uint8_t sensors_bit[] =
      { IO_BV (CONTACT_BACK_LEFT_IO), IO_BV (CONTACT_BACK_RIGHT_IO),
	IO_BV (CONTACT_FRONT_LEFT_IO), IO_BV (CONTACT_FRONT_RIGHT_IO), };
    static const double table_width = 3000.0, table_height = 2100.0;
    static const double stand_x_min = 1500.0 - 759.5,
		 stand_x_max = 1500.0 + 759.5, stand_y = 2100.0 - 522.0;
    PINC = 0;
    unsigned int i;
    double x, y;
    for (i = 0; i < UTILS_COUNT (sensors); i++)
      {
	/* Compute absolute position. */
	simu_compute_absolute_position (sensors[i][0], sensors[i][1], &x, &y);
	if (x >= 0.0 && x < table_width && y >= 0.0 && y < table_height
	    && (x < stand_x_min || x >= stand_x_max || y < stand_y))
	    PINC |= sensors_bit[i];
      }
    /** Top zero sensors. */
    if (simu_aux_model[1].th < 0)
	PINC |= IO_BV (CONTACT_AUX1_ZERO_IO);
}

static void
marcel_robot_init (const struct robot_t *robot, struct motor_model_t *main_motor_left,
		   struct motor_model_t *main_motor_right, struct motor_model_t aux_motor[])
{
    aux_motor[0].m.th_min = 0.0;
}

/* Table test for Robospierre. */
int
simu_table_test_robospierre (double p_x, double p_y)
{
    static const double table_width = 3000.0, table_height = 2100.0;
    double x, y;
    simu_compute_absolute_position (p_x, p_y, &x, &y);
    if (x < 0 || y < 0 || x >= table_width || y >= table_height)
	return 0;
    return 1;
}

