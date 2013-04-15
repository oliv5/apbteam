#ifndef aux_traj_h
#define aux_traj_h
/* aux_traj.h */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "cs.h"

/** Auxiliary motor informations. */
struct aux_t
{
    /** Associated control system. */
    control_system_single_t *cs;
    /** Absolute position. */
    int16_t pos;
    /** Trajectory mode. */
    uint8_t traj_mode;
    /** Goto position position. */
    uint32_t goto_pos;
    /** Clamping PWM. */
    int16_t clampin_pwm;
    /** Wait counter. */
    uint16_t wait;
    /** Top zero port input register. */
    volatile uint8_t *zero_pin;
    /** Top zero port bit value. */
    uint8_t zero_bv;
    /** Handle blocking by aux instead of pos. */
    uint8_t handle_blocking;
    /** Reset position after zero is found. */
    int16_t reset_pos;
};

extern struct aux_t aux[AC_ASSERV_AUX_NB];

void
aux_init (void);

void
aux_pos_update (void);

void
aux_traj_speed_start (struct aux_t *aux);

void
aux_traj_goto_start (struct aux_t *aux, uint16_t pos);

void
aux_traj_clamp_start (struct aux_t *aux, int16_t speed, int16_t clampin_pwm);

void
aux_traj_find_zero_start (struct aux_t *aux, int16_t speed, int16_t reset_pos);

void
aux_traj_find_limit_start (struct aux_t *aux, int16_t speed,
			   int16_t reset_pos);

void
aux_traj_update (void);

#endif /* aux_traj_h */
