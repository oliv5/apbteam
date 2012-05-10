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

#define SERVO_ANGLE_MIN 	69
#define SERVO_ANGLE_MAX 	254

#define SERVO_1_ANGLE_POSITION 			0
#define SERVO_ANGLE_POSITION_TOLERANCE 	2
#define SERVO_2_ANGLE_POSITION 			270


#define RISING			1
#define FALLING			-1

/* SERVO ID */
typedef enum
{
	SERVO_1,
	SERVO_2
} TServo_ID;

typedef enum
{
	SERVO_SCANNING_OFF,
	SERVO_SCANNING_FAST_IN_PROGRESS,
	SERVO_SCANNING_FAST_FINISHED,
	SERVO_SCANNING_SLOW_IN_PROGRESS,
	SERVO_SCANNING_SLOW_FINISHED
} TServo_state;

typedef struct
{
	TServo_ID id;
	int16_t top;
	int16_t bottom;
	int scanning_sense;
	TServo_state state;
} servo_s;

/* This function initializes the servo structures */
void servo_structure_init(void);

/* This function initializes low and high level modules for servos */
void servo_init(void);

/* This function initializes the timer used for servomotor signal generation */
void servo_timer1_init(void);

/* This function increases by one unit the angle of the defined servo and returns "angle" value */
int16_t servo_angle_increase(TServo_ID servo_id);

/* This function decreases by one unit the angle of the defined servo and returns "angle" value */
int16_t servo_angle_decrease(TServo_ID servo_id);

/* This function returns the "angle" value of a defined servo */
int16_t servo_get_value(TServo_ID servo_id);

/* This function sets the "angle" value of a defined servo */
int16_t servo_set_value(TServo_ID servo_id,int16_t value);

/* This function returns the current state of a defined servo */
TServo_state servo_get_state(TServo_ID servo_id);

/* This function updates the state of a defined servo */
void servo_set_state(TServo_ID servo_id,TServo_state state);

/* This function returns the scanning sens of the defined servo */
int8_t servo_get_scanning_sense(TServo_ID servo_id);

/* This function inverses the scanning sense of the servo */
void servo_inverse_scanning_sense(TServo_ID servo_id);

#endif
