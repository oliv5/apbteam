#ifndef models_host_h
#define models_host_h
/* models.host.h */
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

#define ECHANT_PERIOD (1.0 / ((double) AC_FREQ / 256 / 256))

/** Define a robot and its peripherals.
 * Encoder characteristics are defined at gearbox output. */
struct robot_t
{
    /** Auxiliary motors, NULL if not present. */
    const struct motor_def_t *aux_motor[AC_ASSERV_AUX_NB];
    /** Number of steps for each auxiliary motor encoder. */
    int aux_encoder_steps[AC_ASSERV_AUX_NB];
    /** Sensor update function. */
    void (*sensor_update) (void);
};

/** Get a pointer to a model by name, or return 0. */
const struct robot_t *
models_get (const char *name);

/** Initialise simulation models. */
void
models_init (const struct robot_t *robot, struct motor_t aux_motor[]);

#endif /* models_host_h */
