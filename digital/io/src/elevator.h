#ifndef elevator_h
#define elevator_h
/* elevator.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Haller
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
 * State of the elevator
 */
extern uint8_t elevator_is_ready;

/**
 * nb puck in the elevator
 */
extern uint8_t nb_puck_in_elvt;

/**
 * new_puck information (from filterbridge)
 */
extern uint8_t elvt_new_puck;

/**
 * elevator orders
 */
extern uint8_t elvt_order;

/**
 * conversion stop/millimeter
 */

#define STEP_BY_MM 72.34

/**
 * pwm constant for elevator doors
 */

#define OPEN_DOOR_PWM 200
#define CLOSE_DOOR_PWM -200
#define TIME_DOORS_PWM 0xcb
#define TIME_LIGHT_DOORS_PWM 0xaa

#endif // elevator_h
