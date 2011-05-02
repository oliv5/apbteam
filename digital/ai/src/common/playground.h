#ifndef playground_h
#define playground_h
/* playground.h */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "defs.h"

/**
 * Define playground constant data and playground macro to define position and
 * angle depending of the current team color.
 */

/** Playground width, mm. */
#define PG_WIDTH 3000

/** Playground length, mm. */
#define PG_LENGTH 2100

/** Distance to playground borders for general movements, mm. */
#define PG_BORDER_DISTANCE 250

/** Return X coordinate, applying symmetry according to team color. */
#define PG_X(x) (team_color ? (x) : PG_WIDTH - (x))

/** Same as PG_Y, but for Y coordinate.  Actually nothing is done, there is no
 * symmetry. */
#define PG_Y(y) (y)

/** Return an angle, applying symmetry according to team color.  Takes degrees as
 * input. */
#define PG_A_DEG(a) \
    (team_color ? POSITION_A_DEG (a) : POSITION_A_DEG (180 - (a)))

/** Initialiser for position_t applying symmetry according to color.  Takes
 * degrees for angle. */
#define PG_POSITION_DEG(x, y, a) \
    { { PG_X (x), PG_Y (y) }, PG_A_DEG (a) }

/** Initialiser for vect_t applying symmetry according to color. */
#define PG_VECT(x, y) \
    (vect_t) { PG_X (x), PG_Y (y) }

#endif /* playground_h */
