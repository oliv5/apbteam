#ifndef intersection_h
#define intersection_h
/* intersection.h */
/* avr.math.geometry - Geometry math module. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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

/** Test intersection between two line segments.  Return non zero if they
 * intersect.
 * - a, b: first line segment vertices.
 * - c, d: second line segment vertices.
 *
 * If AB and CD are parallel or if a vertice is exactly on the other segment,
 * consider there is no intersection. */
uint8_t
intersection_segment_segment (const vect_t *a, const vect_t *b,
			      const vect_t *c, const vect_t *d);

/** Test intersection between a line segment and any line segment defining a
 * polygon.  Return non zero if they intersect.
 * - a, b: line segment vertices.
 * - poly: array of polygon vertices.
 * - poly_size: number of polygon vertices. */
uint8_t
intersection_segment_poly (const vect_t *a, const vect_t *b,
			   const vect_t *poly, uint8_t poly_size);

#endif /* intersection_h */
