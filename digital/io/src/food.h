#ifndef food_h
#define food_h
/* food.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
 * Tools for playing element for Eurobot 2010.
 *
 * Elements are oranges, tomatoes and corns.  We only handle tomatoes and
 * corns.
 */

/** Return whether a food is blocking robot movements.
 * - food: considered food, or 0xff.
 * 0xff is accepted as a convenience for the user, it always return false. */
uint8_t
food_blocking (uint8_t food);

/** Return whether a food is blocking a line segment.
 * - a: line segment first point.
 * - b: line segment second point.
 * - ab: line segment length.
 * - returns: 1 if the path should not be used. */
uint8_t
food_blocking_path (vect_t a, vect_t b, int16_t ab);

/** Choose the best next food to pick.
 * - robot_pos: current robot position. */
uint8_t
food_best (position_t robot_pos);

/** Get position of a food. */
void
food_pos (uint8_t food, vect_t *v);

/** Mark a food has taken. */
void
food_taken (position_t robot_pos);

/** Return shorten distance to be used to pick this food. */
int16_t
food_shorten (uint8_t food);

/** Slow motion distance to be used to pick this food. */
int16_t
food_slow_motion (uint8_t food);

#endif /* food_h */
