#ifndef trap_h
#define trap_h
/* trap.h */
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
 * @file Module to control the traps (using servo module).
 * It is a higher interface to the servo module more simple to use.
 * The traps and the servos motor are organized in the following ways:
 * @verbatim
 *   +---+----+---+----+---+
 * G | L | ML | M | MR | R |
 *   +---+----+---+----+---+
 *   0   1    2 ^ 3    4
 * @endverbatim
 * In the scheme, the target boxes for the balls are identified by letters,
 * and the servo motors that control the traps are identified by a number:
 *  - L: left;
 *  - ML: middle left;
 *  - M: middle;
 *  - MR: middle right;
 *  - R: right.
 * Balls are entering in the system between the 2 and 3 traps, where the ^
 * symbol is.
 * If the ball want to go M, traps 2 and 3 must be vertical.
 * To go to ML, trap 4 must be vertical, 2 must be horizontal and 1 must be
 * vertical.
 * To go to the garbage, trap 3 must be vertical and traps 0, 1 and 2 must be
 * horizontal.
 * There is also a sixth trap to eject all the balls. It is used to put the
 * balls contained in the boxes into the gutter (unlike the garbage).
 */

#include "servo.h"	/* SERVO_NUMBER */

/**
 * Trap high time values for the different positions (horizontal and
 * vertical).
 * We need to declare it as external to be able to use it from the EEPROM
 * module.
 */
extern uint8_t trap_high_time_pos[2][SERVO_NUMBER];

/**
 * Trap module initialization.
 * It initializes the sub-module servo.
 */
void
trap_init (void);

/**
 * List of boxes where to store balls.
 */
typedef enum trap_box_id_e
{
    /** Throw away ball. */
    garbage = 0,
    /** Most left box. */
    out_left_box,
    /** Middle left box. */
    middle_left_box,
    /** Box for the middle slot. */
    middle_box,
    /** Middle right box. */
    middle_right_box,
    /** Most right box. */
    out_right_box,
    /** Count of trap boxes. */
    trap_count,
} trap_box_id_e;

/**
 * Configure traps to open a path to a box.
 * This function will setup all the servo open a path to the desired box.
 * @param box the box where you want the path to lead to.
 */
void
trap_setup_path_to_box (trap_box_id_e box);

/**
 * Set high time value for horizontal and vertical position of a trap.
 * @param servo_id the servo identification number.
 * @param h the horizontal high time value.
 * @param v the vertical high time value.
 */
void
trap_set_high_time (uint8_t servo_id, uint8_t h, uint8_t v);

/**
 * Open the rear panel to eject the balls.
 */
void
trap_open_rear_panel (void);

/**
 * Close the rear panel to eject the balls.
 */
void
trap_close_rear_panel (void);

#endif /* trap_h */
