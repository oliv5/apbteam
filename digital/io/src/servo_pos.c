/* servo_pos.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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

#include "servo_pos.h"

uint8_t servo_pos_high_time[SERVO_NUMBER][SERVO_POS_NUMBER];

void
servo_pos_init (void)
{
    /* Initialize under layer, servo module. */
    servo_init ();
}

void
servo_pos_set_high_time (uint8_t servo_id,
			 uint8_t high_times[SERVO_POS_NUMBER])
{
    uint8_t i;
    /* If servo exists. */
    if (servo_id < SERVO_NUMBER)
      {
	/* For each position. */
	for (i = 0; i < SERVO_POS_NUMBER; i++)
	    servo_pos_high_time[servo_id][i] = high_times[i];
      }
}

/**
 * Move a servo to a specific position.
 * @param  servo_id  the id of the servo to move.
 * @param  position  the position identifier where to move the servo.
 */
void
servo_pos_move_to (uint8_t servo_id, uint8_t position)
{
      servo_set_high_time (servo_id, servo_pos_high_time[servo_id][position]);
}
