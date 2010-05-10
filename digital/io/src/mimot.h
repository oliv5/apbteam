#ifndef mimot_h
#define mimot_h
/* mimot.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
#include "asserv.h"

/**
 * Provide interface to mimot board using the TWI protocol.
 */

/** Slave number in the twi_master list. */
#define MIMOT_SLAVE 1

/** Mimot TWI address. */
#define MIMOT_TWI_ADDRESS 6

/** Length of status buffer (not including CRC). */
#define MIMOT_STATUS_LENGTH 7

/** Initialise module. */
void
mimot_init (void);

/** Called when a new status buffer is received, update the mimot
 * information. */
void
mimot_status_cb (uint8_t *status);

/** Return motor0 last command status. */
asserv_status_e
mimot_motor0_cmd_status (void);

/** Return motor1 last command status. */
asserv_status_e
mimot_motor1_cmd_status (void);

/** Get motor0 position in steps. */
uint16_t
mimot_get_motor0_position (void);

/** Get motor1 position in steps. */
uint16_t
mimot_get_motor1_position (void);

/** Reset mimot board. */
void
mimot_reset (void);

/** Move motor0 to absolute position in steps. */
void
mimot_move_motor0_absolute (uint16_t position, uint8_t speed);

/** Move motor1 to absolute position in steps. */
void
mimot_move_motor1_absolute (uint16_t position, uint8_t speed);

/** Reset motor0 to zero position. */
void
mimot_motor0_zero_position (int8_t speed);

/** Reset motor1 to zero position. */
void
mimot_motor1_zero_position (int8_t speed);

/** Clamp motor0. */
void
mimot_motor0_clamp (int8_t speed, int16_t pwm);

/** Clamp motor1. */
void
mimot_motor1_clamp (int8_t speed, int16_t pwm);

#endif /* mimot_h */
