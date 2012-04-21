/* motor.h */
/* Motor control. {{{
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

#ifndef _MOTOR_H
#define _MOTOR_H

typedef enum
{
	MOTOR_STOPPED,
	MOTOR_IN_ROTATION
} TMotor_state;

/* This function initializes the motor control output */
void motor_init(void);

/* This function starts the motor rotation */
void motor_start(void);

/* This function stops the motor rotation */
void motor_stop(void);

/* This function returns the motor state */
TMotor_state motor_get_state(void);

#endif
