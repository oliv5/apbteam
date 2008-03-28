/* trap.c */
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

#include "common.h"
#include "trap.h"

/**
 * @todo
 *   - how to manage traps collision for the 3 and 4.
 *   - who map servo/pin to trap?
 */

/**
 * @defgroup TrapPrivate Trap module private variables and functions
 * declarations
 * @{
 */

/**
 * Possible positions of a trap.
 * It is used for the index of the table @see trap_high_time_pos.
 */
typedef enum trap_position_e
{
    /** Horizontal. */
    horizontal = 0,
    /** Vertical. */
    vertical,
    
    /** Length of the enum, always left it at the end. */
    lenght
} trap_position_e;

/**
 * Trap high time values for the different positions (horizontal and vertical).
 * In this two dimensions table, the first index represent horizontal or
 * vertical values.
 */
uint8_t trap_high_time_pos[lenght][SERVO_NUMBER];

/** @} */

/* Trap module initialization. */
void
trap_init (void)
{
    /* Initialize servo module */
    servo_init ();
}

/* Configure traps to open a path to a box. */
void
trap_setup_path_to_box (trap_box_id_e box)
{
    switch (box)
    {
    case garbage:
      /* 0:H, 1:H, 2:H, 3:V, 4:? */
      servo_set_high_time (0, trap_high_time_pos[horizontal][0]);
      servo_set_high_time (1, trap_high_time_pos[horizontal][1]);
      servo_set_high_time (2, trap_high_time_pos[horizontal][2]);
      servo_set_high_time (3, trap_high_time_pos[vertical][3]);
      break;
    case out_left_box:
      /* 0:V, 1:H, 2:H, 3:V, 4:? */
      servo_set_high_time (0, trap_high_time_pos[vertical][0]);
      servo_set_high_time (1, trap_high_time_pos[horizontal][1]);
      servo_set_high_time (2, trap_high_time_pos[horizontal][2]);
      servo_set_high_time (3, trap_high_time_pos[vertical][3]);
      break;
    case middle_left_box:
      /* 0:?, 1:V, 2:H, 3:V, 4:? */
      servo_set_high_time (1, trap_high_time_pos[vertical][1]);
      servo_set_high_time (2, trap_high_time_pos[horizontal][2]);
      servo_set_high_time (3, trap_high_time_pos[vertical][3]);
      break;
    case middle_box:
      /* 0:?, 1:?, 2:V, 3:V, 4:? */
      servo_set_high_time (2, trap_high_time_pos[vertical][2]);
      servo_set_high_time (3, trap_high_time_pos[vertical][3]);
      break;
    case middle_right_box:
      /* 0:?, 1:?, 2:V, 3:H, 4:V */
      servo_set_high_time (2, trap_high_time_pos[vertical][2]);
      servo_set_high_time (3, trap_high_time_pos[horizontal][3]);
      servo_set_high_time (4, trap_high_time_pos[vertical][4]);
      break;
    case out_right_box:
      /* 0:?, 1:?, 2:V, 3:H, 4:H */
      servo_set_high_time (2, trap_high_time_pos[vertical][2]);
      servo_set_high_time (3, trap_high_time_pos[horizontal][3]);
      servo_set_high_time (4, trap_high_time_pos[horizontal][4]);
      break;
    }
}

/* Set high time value for horizontal and vertical position of a trap. */
void
trap_set_high_time (uint8_t servo_id, uint8_t horizontal, uint8_t vertical)
{
    if (servo_id < SERVO_NUMBER)
      {
	trap_high_time_pos[horizontal][servo_id] = horizontal;
	trap_high_time_pos[vertical][servo_id] = vertical;
      }
}
