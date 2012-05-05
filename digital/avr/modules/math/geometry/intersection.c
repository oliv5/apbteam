/* intersection.c */
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
#include "common.h"
#include "intersection.h"

/** Compare a and b to determine if a / b is in ]0:1[.  Return non zero if
 * true. */
static uint8_t
intersection_div_is_in_0_1 (int32_t a, int32_t b)
{
    assert (b != 0);
    /* Test sign, a / b < 0 if different. */
    if ((a ^ b) & 0x80000000)
	return 0;
    else if (a == 0)
	return 0;
    else if (a > 0)
	return a < b;
    else
	return a > b;
}

uint8_t
intersection_segment_segment (const vect_t *a, const vect_t *b,
			      const vect_t *c, const vect_t *d)
{
    /*
     * For each point P on the line segment [AB], there is a real u in [0, 1]
     * for which P = A + u (B - A)
     *
     * An intersection point must be on both line segments:
     *
     * A + u (B - A) = C + v (D - C)
     *
     * a.x + u (b.x - a.x) = c.x + v (d.x - c.x)
     * a.y + u (b.y - a.y) = c.y + v (d.y - c.y)
     *
     *     (c.x - a.x) (d.y - c.y) - (d.x - c.x) (c.y - a.y)
     * u = -------------------------------------------------
     *     (b.x - a.x) (d.y - c.y) - (d.x - c.x) (b.y - a.y)
     *
     *     (c.x - a.x) (b.y - a.y) - (b.x - a.x) (c.y - a.y)
     * v = -------------------------------------------------
     *     (b.x - a.x) (d.y - c.y) - (d.x - c.x) (b.y - a.y)
     *
     * u = (vac.normal . vcd) / (vab.normal . vcd)
     * v = (vac.normal . vab) / (vab.normal . vcd)
     *
     * If vab.normal . vcd is 0, AB and CD are parallel.
     */
    vect_t vab = *b; vect_sub (&vab, a);
    vect_t vcd = *d; vect_sub (&vcd, c);
    int32_t den = vect_normal_dot_product (&vab, &vcd);
    if (den == 0)
	return 0;
    else
      {
	vect_t vac = *c; vect_sub (&vac, a);
	int32_t unum = vect_normal_dot_product (&vac, &vcd);
	if (!intersection_div_is_in_0_1 (unum, den))
	    return 0;
	else
	  {
	    int32_t vnum = vect_normal_dot_product (&vac, &vab);
	    return intersection_div_is_in_0_1 (vnum, den);
	  }
      }
}

uint8_t
intersection_segment_poly (const vect_t *a, const vect_t *b,
			   const vect_t *poly, uint8_t poly_size)
{
    uint8_t i;
    assert (poly_size > 0);
    for (i = 0; i < poly_size - 1; i++)
      {
	if (intersection_segment_segment (a, b, &poly[i], &poly[i + 1]))
	    return 1;
      }
    if (intersection_segment_segment (a, b, &poly[i], &poly[0]))
	return 1;
    return 0;
}

