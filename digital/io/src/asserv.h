#ifndef asserv_h
#define asserv_h
/* asserv.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Dufour Jérémy
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

/**
 * @file Control the asserv board from io using the TWI protocol.
 * This files contains the 'public' functions to send commands to the asserv
 * board using a protocol over TWI communication.
 * @see trunk/digital/io/doc/proto_asserv.txt
 * @todo
 *  - protect code from sending a command when the previous one is not
 *  finished yet.
 *  - a consequence of previous item is that commands can failed. Maybe all
 *  return values should be 'uint8_t'.
 */

#include "common.h"

/**
 * Initialize the asserv control module.
 * This functions does not initialize the asserv board, but the underling
 * communication protocol used to communicate with the asserv (TWI).
 */
void
asserv_init (void);

/**
 * Reset the asserv board.
 * Other class command.
 */
void
asserv_reset (void);

/**
 * Free the motors (stop controlling them).
 * Other class command.
 */
void
asserv_free_motor (void);

/**
 * Stop the motor (and the bot).
 * Other class command.
 */
void
asserv_stop_motor (void);

/**
 * Move linearly.
 * Move class command.
 * @param distance the distance to move (in step).
 */
void
asserv_move_linearly (int32_t distance);

/**
 * Move angularly (turn).
 * Move class command.
 * @param angle the angle to turn.
 */
void
asserv_move_angularly (int16_t angle);

/**
 * Go to the wall (moving backward).
 * Move class command.
 */
void
asserv_go_to_the_wall (void);

/**
 * Move forward to approach a gutter.
 * Move class command.
 */
void
asserv_go_to_gutter (void);

/**
 * Move the arm.
 * A complete rotation correspond to 5000 steps.
 * Arm class command.
 * @param position desired goal position (in step).
 * @param speed speed of the movement.
 */
void
asserv_move_arm (uint16_t position, uint8_t speed);

/**
 * Set current X position.
 * Other class command.
 * @param x X position.
 */
void
asserv_set_x_position (int32_t x);

/**
 * Set current Y position.
 * Other class command.
 * @param y Y position.
 */
void
asserv_set_y_position (int32_t y);

/**
 * Set current angular position.
 * Other class command.
 * @param angle angular position.
 */
void
asserv_set_angle_position (int16_t angle);

/**
 * Set speeds of movements.
 * Other class command.
 * @param linear_high linear high speed
 * @param angular_high angular high speed
 * @param linear_low linear low speed
 * @param angular_low angular low speed
 */
void
asserv_set_speed (uint8_t linear_high, uint8_t angular_high,
		  uint8_t linear_low, uint8_t angular_low);

#endif /* asserv_h */
