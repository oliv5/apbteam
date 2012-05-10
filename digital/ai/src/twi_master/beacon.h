#ifndef beacon_h
#define beacon_h
/* beacon.h */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "defs.h"

/** Interface with beacon board using the TWI protocol. */

/** Beacon TWI address. */
#define BEACON_TWI_ADDRESS 10

/** Length of status buffer (not including CRC). */
#define BEACON_STATUS_LENGTH (3 + 5 * AC_BEACON_POSITION_NB)

/** Initialise module. */
void
beacon_init (void);

/** Called when a new status buffer is received, update the beacon
 * information. */
void
beacon_status_cb (uint8_t *status);

/** Turn on (1) or off (0). */
void
beacon_on (uint8_t on_off);

/** Communicate the number of opponent robots. */
void
beacon_robot_nb (uint8_t robot_nb);

/** Send my position, along with other informations. */
void
beacon_send_position (vect_t *position);

/** Get a detected position, return trust (0 for invalid, 100 for full
 * trust). */
uint8_t
beacon_get_position (uint8_t index, vect_t *position);

#endif /* beacon_h */
