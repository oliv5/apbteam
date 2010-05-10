/* distance.c */
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
#include "common.h"
#include "distance.h"

#include "modules/utils/utils.h"
#include "modules/math/fixed/fixed.h"

int16_t
distance_point_point (vect_t *a, vect_t *b)
{
    int32_t dx = b->x - a->x;
    int32_t dy = b->y - a->y;
    return fixed_sqrt_ui32 (dx * dx + dy * dy);
}

int16_t
distance_segment_point (vect_t *a, vect_t *b, vect_t *p)
{
    int16_t proj;
    /* The space is split in three regions delimited by the two parallel lines
     * going through A and B, perpendicular to the segment:
     *
     *     |       |
     *     |   2   |
     *     |       |
     *  1  A=======B  3
     *     |       |
     *     |   2   |
     *     |       |
     *
     * If P is in region 2, distance is the distance between P and the AB
     * line.
     *
     * If P is in region 1, distance is |AP|.  If P is in region 3, distance
     * is |BP|.
     *
     * If vAP . vAB <= 0, P is in region 1.  If vBP . vAB > 0, P is in region
     * 3.  Else, it is in region 2.
     *
     * Distance to line is determined using dot product between vn, a normal
     * of vAB and vBP. */
    vect_t vab, vap, vbp;
    /* Region 1? */
    vab = *b; vect_sub (&vab, a);
    vap = *p; vect_sub (&vap, a);
    if (vect_dot_product (&vab, &vap) <= 0)
	return vect_norm (&vap);
    /* Region 3? */
    vbp = *p; vect_sub (&vbp, b);
    if (vect_dot_product (&vab, &vbp) > 0)
	return vect_norm (&vbp);
    /* Else, region 2. */
    proj = vect_normal_dot_product (&vab, &vbp) / vect_norm (&vab);
    return UTILS_ABS (proj);
}

