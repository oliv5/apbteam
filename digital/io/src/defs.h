#ifndef defs_h
#define defs_h
/* defs.h */
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

#endif /* defs_h */
