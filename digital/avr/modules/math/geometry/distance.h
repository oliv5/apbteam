#ifndef distance_h
#define distance_h
/* distance.h */
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
#include "modules/math/geometry/vect.h"

/** Compute distance between two points.
 * - a, b: points. */
int16_t
distance_point_point (const vect_t *a, const vect_t *b);

/** Compute distance between a segment and a point.
 * - a: segment first vertice.
 * - b: segment second vertice.
 * - p: point. */
int16_t
distance_segment_point (const vect_t *a, const vect_t *b, const vect_t *p);

#endif /* distance_h */
