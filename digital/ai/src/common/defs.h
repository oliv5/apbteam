#ifndef defs_h
#define defs_h
/* defs.h */
/* ai - Robot Artificial Intelligence. {{{
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
#include "modules/math/geometry/geometry.h"
#include "modules/math/geometry/vect.h"

/** General purpose defines. */

/** Structure defining a position with an angle. */
struct position_t
{
    /** Cartesian position in millimeters, (0, 0) is at bottom left. */
    vect_t v;
    /** Angle, counter-clockwise, [0, 1), f0.16.
     * For example, 0x8000 means 0.5, which means 180 degrees. */
    uint16_t a;
};
typedef struct position_t position_t;

/** Convert degrees to an angle usable in position_t. */
#define POSITION_A_DEG(a) G_ANGLE_UF016_DEG (a)

/** No particular direction. */
#define DIRECTION_NONE 0
/** Forward direction, along the robot X axis. */
#define DIRECTION_FORWARD 1
/** Backward, opposite the robot X axis. */
#define DIRECTION_BACKWARD 2

/** Team color, determine the start zone side. */
enum team_color_e
{
    TEAM_COLOR_RIGHT = 0,
    TEAM_COLOR_LEFT = 1
};

/** Current team color, to be read at start up. */
extern enum team_color_e team_color;

#endif /* defs_h */
