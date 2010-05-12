#ifndef playground_h
#define playground_h
// playground.h
// io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
//
// Copyright (C) 2008 Dufour Jérémy
//
// Robot APB Team/Efrei 2004.
//        Web: http://assos.efrei.fr/robot/
//      Email: robot AT efrei DOT fr
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}

/**
 * @file Some defines for the playground.
 * For example, you can find the size of the table, the positions of the
 * distributors, ...
 * A few important remarks:
 *   - the (0,0) position is the bottom left position on the table (when you
 *   see it with the two start zone at the top of the scheme).
 */

#include "defs.h"
#include "bot.h"

/**
 * The width of the table, in millimeters.
 */
#define PG_WIDTH 3000

/**
 * The length of the table, in millimeters.
 */
#define PG_LENGTH 2100

/**
 * The distance from table border for movements.
 */
#define PG_BORDER_DISTANCE 250

/**
 * Considering there is a symmetry axis on X, this macro will compute the
 * value for on the X axis depending on the color.
 */
#define PG_X(x) (bot_color ? (x) : PG_WIDTH - (x))

/** Same as PG_Y, but for Y coordinate.  Actually nothing is done, there is no
 * symmetry. */
#define PG_Y(y) (y)

/**
 * Considering there is a symmetry axis on X, this macro will compute the
 * value of the angle depending on the color.
 *
 * Takes degrees as input.
 */
#define PG_A_DEG(a) \
    (bot_color ? POSITION_A_DEG (a) : POSITION_A_DEG (180 - (a)))

/** Initialiser for position_t applying symmetry according to color.  Takes
 * degrees for angle. */
#define PG_POSITION_DEG(x, y, a) \
    { { PG_X (x), PG_Y (y) }, PG_A_DEG (a) }

/** Initialiser for vect_t applying symmetry according to color. */
#define PG_VECT(x, y) \
    (vect_t) { PG_X (x), PG_Y (y) }

/**
 * Start zone.
 */
#define PG_START_ZONE_LENGTH 500
#define PG_START_ZONE_WIDTH 500

/** Size of the unclimbable slope zone (Eurobot 2010). */
#define PG_SLOPE_WIDTH (500 + 519 + 500)
#define PG_SLOPE_LENGTH (500 + 22)

/** Start of field zone. */
#define PG_FIELD_Y_MAX 1128

#endif // playground_h
