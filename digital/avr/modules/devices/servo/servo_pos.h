#ifndef servo_pos_h
#define servo_pos_h
/* servo.pos.h */
/* avr.devices.servo - Servo AVR module. {{{
 *
 * Copyright (C) 2009 Dufour Jérémy
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
 * Module to control two positions of the servo motor.
 * It's an higher interface that uses the servo module but remember the
 * positions (for example, two positions, open and close) that they can take.
 */

#include "servo.h"

/**
 * Number of positions the servo can take.
 * @warning if you change this define, you must update the key value in the
 * EEPROM module. Otherwise, everything will fail!
 */
#define SERVO_POS_NUMBER 2

/**
 * Table to store the correspondence between the positions and the high time.
 */
extern uint8_t servo_pos_high_time[SERVO_NUMBER][SERVO_POS_NUMBER];

/**
 * Initialize the servo with positions module.
 * @note it also initialize the servo module for you.
 */
void
servo_pos_init (void);

/**
 * Set the high times of a servo for positions.
 * @param  servo_id  the ID of the servo.
 * @param  high_times  the different high times for the positions of the
 * servo.
 */
void
servo_pos_set_high_time (uint8_t servo_id,
			 uint8_t high_times[SERVO_POS_NUMBER]);

/**
 * Move a servo to a specific position.
 * @param  servo_id  the id of the servo to move.
 * @param  position  the position identifier where to move the servo.
 */
void
servo_pos_move_to (uint8_t servo_id, uint8_t position);

#endif /* servo_pos_h */
