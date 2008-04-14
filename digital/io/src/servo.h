#ifndef servo_h
#define servo_h
/* servo.h */
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

#include "common.h"	// uint8_t

/**
 * @file Module to control servo motors.
 * This module contains low-level functions to control the servo motors. If
 * you want to control the traps to store the balls at a specific place, have
 * a look at the trap module.
 * It uses the timer/counter 2 (8-bit).
 *
 * Servo motors can be controlled by the time the input signal spend at its
 * high state. For example, if the signal sent to the servo motor only spend
 * 1ms at the high state, it will have a 0° angle position. If the signal
 * stays for 1.5ms at high state, it will have a 90° angle position.
 * To manage all servo motors in a "one time shot", we need to use the
 * timer/counter 2 of the ATmega128 and its overflow.
 * We setup the timer/counter to the value of its overflow minus the time the
 * input signal of the servo need to spend at high state ; we put the input
 * signal of this servo motor to the high value. When the timer overflows, we
 * put it back to the low state. We go to the next servo motors and do the
 * same algorithm. When the all servos motor have been taken care of, we
 * set-up the timer to overflow a certain number of times to wait before
 * restarting the whole cycle.
 */

/**
 * Number of servos motor managed by this module.
 * If you change it, you _must_ update the key of the eeprom module!
 */
#define SERVO_NUMBER 6

/**
 * Minimum high time for servos.
 */
#define SERVO_HIGH_TIME_MIN 0x24

/**
 * Maximum high time for servos.
 */
#define SERVO_HIGH_TIME_MAX 0x88

/**
 * Initialize servo module.
 * This functions put the pins of the servos motor in the right direction,
 * initialize the timer/counter 2 and some internals stuff.
 */
void
servo_init (void);

/**
 * Set the high time of the input signal of a servo (and its position).
 * @param servo the servo to change the position.
 * @param high_time the high time we want the input signal to spend at the
 * high state to set the servo motor to a position.  A zero will let the servo
 * floating.
 */
void
servo_set_high_time (uint8_t servo, uint8_t high_time);

/**
 * Get the high time of the servo.
 * @param servo the servo to get the position of.
 * @return the current position of the servo.
 */
uint8_t
servo_get_high_time (uint8_t servo);

#endif /* servo_h */
