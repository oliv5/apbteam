/* servo.h */
/* Beacon servomotor management. {{{
 *
 * Copyright (C) 2012 Florent Duchon
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

#ifndef _SERVO_H
#define _SERVO_H

#define SERVO_1_ANGLE_INIT 	150
#define SERVO_1_ANGLE_MIN 	69
#define SERVO_1_ANGLE_MAX 	300
#define SERVO_2_ANGLE_INIT 	150
#define SERVO_2_ANGLE_MIN 	69
#define SERVO_2_ANGLE_MAX 	300

/* SERVO ID */
typedef enum
{
	SERVO_1=1,
	SERVO_2
} TServo_ID;

/* This function initializes the timer used for servomotor signal generation */
void servo_timer1_init(void);

/* This function increase by one unit the angle of the defined servo and returns "angle" value */
int servo_angle_increase(TServo_ID servo_id);

/* This function decrease by one unit the angle of the defined servo and returns "angle" value*/
int servo_angle_decrease(TServo_ID servo_id);

#endif