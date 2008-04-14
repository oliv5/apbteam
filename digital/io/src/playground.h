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

#include "giboulee.h"

/**
 * The width of the table, in millimeters.
 */
#define PG_WIDTH 3000

/**
 * The height of the table, in millimeters.
 */
#define PG_HEIGHT 2100

/**
 * Considering there is a symmetry axis on X, this macro will compute the
 * value for on the X axis depending on the color.
 */
#define PG_X_VALUE_COMPUTING(x) \
    (bot_color ? x : PG_WIDTH - x)

/**
 * Considering there is a symmetry axis on X, this macro will compute the
 * value of the angle depending on the color.
 */
#define PG_A_VALUE_COMPUTING(a) \
    (bot_color ? a : (a + BOT_ANGLE_DEGREE * 180))

/**
 * The position where to reset the bot when it starts, depending on the color.
 */
#define PG_X_START (PG_X_VALUE_COMPUTING (200))
#define PG_Y_START (PG_HEIGHT - 70)
#define PG_A_START (BOT_ANGLE_DEGREE * 90)

/**
 * The distance to remove from the real position of the distributor before
 * doing the "go to the distributor".
 */
#define PG_DISTANCE_DISTRIBUTOR 100

/**
 * The position of our ice distributor minus the distance to remove (only for the
 * X axis).
 */
#define PG_DISTRIBUTOR_ICE_OUR_X \
    (PG_X_VALUE_COMPUTING (0 + PG_DISTANCE_DISTRIBUTOR))
#define PG_DISTRIBUTOR_ICE_OUR_Y 1350
#define PG_DISTRIBUTOR_ICE_OUR_A \
    (PG_A_VALUE_COMPUTING (BOT_ANGLE_DEGREE * 180))

/**
 * The position of adverse ice distributor minus the distance to remove (only
 * for the X axis).
 */
#define PG_DISTRIBUTOR_ICE_ADVERSE_X \
    (PG_X_VALUE_COMPUTING (PG_WIDTH - PG_DISTANCE_DISTRIBUTOR))
#define PG_DISTRIBUTOR_ICE_ADVERSE_Y 1350
#define PG_DISTRIBUTOR_ICE_ADVERSE_A \
    (PG_A_VALUE_COMPUTING (0))

/**
 * The position of our distributor minus the distance to remove (only for the
 * Y axis).
 */
#define PG_DISTRIBUTOR_SAMPLE_OUR_X \
    (PG_X_VALUE_COMPUTING (700))
#define PG_DISTRIBUTOR_SAMPLE_OUR_Y (PG_HEIGHT - PG_DISTANCE_DISTRIBUTOR)
#define PG_DISTRIBUTOR_SAMPLE_OUR_A (BOT_ANGLE_DEGREE * 270)

/**
 * The position of the gutter.
 */
#define PG_GUTTER_X (PG_X_VALUE_COMPUTING (2250))
#define PG_GUTTER_Y (100)
#define PG_GUTTER_A (BOT_ANGLE_DEGREE * 90)

#endif // playground_h
