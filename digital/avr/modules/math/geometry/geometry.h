#ifndef geometry_h
#define geometry_h
/* geometry.h */
/* avr.math.geometry - Geometry math module. {{{
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
#include "modules/math/math.h"

/** Compute an angle from radians.  This is supposed to be used with
 * constants. */
#define G_ANGLE_UF016_RAD(a) \
    ((uint16_t) /* Final type is uint16_t, we want to drop MSB. */ \
     (int32_t) /* Do no want funny compiler trick when converting from \
		  double to integer (overflow detection). */ \
     ((a) >= 0 /* Need rounding. */ \
      ? ((a) * 0x8000l + M_PI / 2) / M_PI \
      : ((a) * 0x8000l - M_PI / 2) / M_PI \
     ))

/** Compute an angle from degrees.  This is supposed to be used with
 * constants. */
#define G_ANGLE_UF016_DEG(a) \
    ((uint16_t) /* Final type is uint16_t, we want to drop MSB. */ \
     (int32_t) /* Do no want funny compiler trick when converting from \
		  double to integer (overflow detection). */ \
     ((a) >= 0 /* Need rounding. */ \
      ? ((a) * 0x10000l + 180) / 360 \
      : ((a) * 0x10000l - 180) / 360 \
     ))

#endif /* geometry_h */
