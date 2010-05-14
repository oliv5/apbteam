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
#include "bot.h"
#include "playground.h"

#include "modules/utils/utils.h"
#include "modules/math/geometry/distance.h"
#include "modules/trace/trace.h"
#include "events.h"

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

static int32_t
food_score (position_t robot_pos, uint8_t food)
{
    int32_t score = 0;
    vect_t v;
    assert (food < UTILS_COUNT (food_table));
    /* Type of food. */
    if (food_table[food].type == FOOD_TYPE_TOMATO)
	score += 100;
    else
	score -= 200;
    /* Distance to robot. */
    food_pos (food, &v);
    int32_t dr = distance_point_point (&v, &robot_pos.v);
    score += dr * -1;
    /* Alignment with robot. */
    if (dr > 100)
      {
	vect_t vr = v; vect_sub (&vr, &robot_pos.v);
	vect_t u; vect_from_polar_uf016 (&u, 100, robot_pos.a);
	int32_t dp = vect_dot_product (&u, &vr);
	int32_t align = dp / dr;
	score += align;
      }
    /* Distance to unloading area. */
    /* Avoid food near border. */
    if (food == 15 || food == 1)
	score -= 300;
    if (v.y < BOT_SIZE_RADIUS)
	score -= 6000;
    /* Done. */
    return score;
}

uint8_t
food_best (position_t robot_pos)
{
    uint8_t i;
    int32_t score;
    uint8_t best = 0xff;
    int32_t best_score = 0;
    for (i = 0; i < UTILS_COUNT (food_table); i++)
      {
	if (!food_table[i].valid)
	    continue;
	score = food_score (robot_pos, i);
	if (best == 0xff || best_score < score)
	  {
	    best = i;
	    best_score = score;
	  }
      }
    TRACE (TRACE_FOOD__BEST, best);
    if (best != 0xff)
	food_table[best].valid = 0;
    return best;
}

void
food_pos (uint8_t food, vect_t *v)
{
    assert (food < UTILS_COUNT (food_table));
    *v = food_table[food].pos;
}

int16_t
food_shorten (uint8_t food)
{
    assert (food < UTILS_COUNT (food_table));
    int16_t shorten = 0;
    /* Corns. */
    if (food_table[food].type == FOOD_TYPE_CORN)
	shorten = BOT_SIZE_FRONT + 70;
    /* Food on playground sides. */
    if (food <= 4 || (food >= 14 && food <= 18))
	shorten = BOT_SIZE_FRONT;
    return shorten;
}

void
food_taken (position_t robot_pos)
{
    uint8_t i;
    for (i = 0; i < UTILS_COUNT (food_table); i++)
      {
	vect_t v;
	food_pos (i, &v);
	if (robot_pos.v.x > v.x - 450 / 2
	    && robot_pos.v.x < v.x + 450 / 2
	    && robot_pos.v.y > v.y - 250 / 2
	    && robot_pos.v.y < v.y + 250 / 2)
	  {
	    food_table[i].valid = 0;
	    TRACE (TRACE_FOOD__TAKEN, i);
	    break;
	  }
      }
}

int16_t
food_slow_motion (uint8_t food)
{
    assert (food < UTILS_COUNT (food_table));
    if (food_table[food].type == FOOD_TYPE_CORN)
	return 50;
    else
	return 0;
}

