/* food.c */
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
#include "common.h"
#include "defs.h"
#include "food.h"

#include "modules/utils/utils.h"

/** Maximum distance from a line segment to food center so that the robot is
 * able to pick a food. */
#define FOOD_NEAR_MM (310 / 2 - 50 - 30)

/** Minimum distance from a line segment to food center so that the robot do
 * not touch the food. */
#define FOOD_FAR_MM (450 / 2 - 10)

/** Food type. */
enum food_type_t
{
    FOOD_TYPE_TOMATO,
    FOOD_TYPE_CORN,
};

/** Food information. */
struct food_t
{
    /** Whether this food is still here or not. */
    uint8_t valid;
    /** Food position. */
    vect_t pos;
    /** Food type.  Corn can be good or bad. */
    enum food_type_t type;
};

/** Information gathered on food. */
struct food_t food_table[] =
{
    /* {{{ */
    /* First half. */
      { 1, { 150, 128 + 5 * 250 }, FOOD_TYPE_CORN },
      { 1, { 150, 128 + 4 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 150, 128 + 3 * 250 }, FOOD_TYPE_CORN },
      { 1, { 150, 128 + 2 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 150, 128 + 1 * 250 }, FOOD_TYPE_CORN },

      { 1, { 150 + 1 * 450, 128 + 4 * 250 }, FOOD_TYPE_CORN },
      { 1, { 150 + 1 * 450, 128 + 3 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 150 + 1 * 450, 128 + 2 * 250 }, FOOD_TYPE_CORN },
      { 1, { 150 + 1 * 450, 128 + 1 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 150 + 1 * 450, 128 + 0 * 250 }, FOOD_TYPE_CORN },

      { 1, { 150 + 2 * 450, 128 + 3 * 250 }, FOOD_TYPE_CORN },
      { 1, { 150 + 2 * 450, 128 + 2 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 150 + 2 * 450, 128 + 1 * 250 }, FOOD_TYPE_CORN },
      { 1, { 150 + 2 * 450, 128 + 0 * 250 }, FOOD_TYPE_TOMATO },

    /* Second half. */
      { 1, { 3000 - 150, 128 + 5 * 250 }, FOOD_TYPE_CORN },
      { 1, { 3000 - 150, 128 + 4 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 3000 - 150, 128 + 3 * 250 }, FOOD_TYPE_CORN },
      { 1, { 3000 - 150, 128 + 2 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 3000 - 150, 128 + 1 * 250 }, FOOD_TYPE_CORN },

      { 1, { 3000 - 150 - 1 * 450, 128 + 4 * 250 }, FOOD_TYPE_CORN },
      { 1, { 3000 - 150 - 1 * 450, 128 + 3 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 3000 - 150 - 1 * 450, 128 + 2 * 250 }, FOOD_TYPE_CORN },
      { 1, { 3000 - 150 - 1 * 450, 128 + 1 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 3000 - 150 - 1 * 450, 128 + 0 * 250 }, FOOD_TYPE_CORN },

      { 1, { 3000 - 150 - 2 * 450, 128 + 3 * 250 }, FOOD_TYPE_CORN },
      { 1, { 3000 - 150 - 2 * 450, 128 + 2 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 3000 - 150 - 2 * 450, 128 + 1 * 250 }, FOOD_TYPE_CORN },
      { 1, { 3000 - 150 - 2 * 450, 128 + 0 * 250 }, FOOD_TYPE_TOMATO },

    /* Center. */
      { 1, { 3000 / 2, 128 + 3 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 3000 / 2, 128 + 2 * 250 }, FOOD_TYPE_CORN },
      { 1, { 3000 / 2, 128 + 1 * 250 }, FOOD_TYPE_TOMATO },
      { 1, { 3000 / 2, 128 + 0 * 250 }, FOOD_TYPE_CORN },
    /* }}} */
};

uint8_t
food_blocking (uint8_t food)
{
    assert (food < UTILS_COUNT (food_table) || food == 0xff);
    return food != 0xff && food_table[food].type == FOOD_TYPE_CORN;
}

uint8_t
food_blocking_path (vect_t a, vect_t b, int16_t ab)
{
    uint8_t i;
    /* For each obstacle, try to find an intersection. */
    for (i = 0; i < UTILS_COUNT (food_table); i++)
      {
	if (food_table[i].valid)
	  {
	    /* Compute square of distance to obstacle, see
	     * distance_segment_point in modules/math/geometry for the method
	     * explanation. */
	    int32_t absq = (int32_t) ab * ab;
	    vect_t vab = b; vect_sub (&vab, &a);
	    vect_t vao = food_table[i].pos; vect_sub (&vao, &a);
	    int32_t dp = vect_dot_product (&vab, &vao);
	    int32_t dsq;
	    if (dp < 0)
	      {
		dsq = vect_dot_product (&vao, &vao);
	      }
	    else if (dp > absq)
	      {
		vect_t vbo = food_table[i].pos; vect_sub (&vbo, &b);
		dsq = vect_dot_product (&vbo, &vbo);
	      }
	    else
	      {
		vect_t vabn = vab; vect_normal (&vabn);
		dsq = vect_dot_product (&vabn, &vao) / ab;
		dsq *= dsq;
	      }
	    /* Compare with square of authorised distance. */
	    if (dsq < (int32_t) FOOD_FAR_MM * FOOD_FAR_MM
		&& (food_table[i].type != FOOD_TYPE_TOMATO
		    || dsq > (int32_t) FOOD_NEAR_MM * FOOD_NEAR_MM))
		return 1;
	  }
      }
    return 0;
}

