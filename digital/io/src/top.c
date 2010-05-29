/* top.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Haller
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
#include "top.h"
#include "food.h"
#include "asserv.h"
#include "loader.h"
#include "move.h"
#include "chrono.h"
#include "playground.h"

#include "modules/utils/utils.h"

uint8_t top_food;

/** Maximum elements to load before unloading. */
#define TOP_LOADER_MAX 3
/** Maximum elements to load before unloading when far from unload area. */
#define TOP_LOADER_MAX_FAR 4
/** Time to reserve for unloading at end of round. */
#define TOP_TIME_LIMIT_MS 20000ll

void
top_init (void)
{
}

uint8_t
top_collect (uint8_t force)
{
    uint8_t loader_max;
    position_t robot_position;
    asserv_get_position (&robot_position);
    if (UTILS_ABS (PG_X (PG_WIDTH) - robot_position.v.x) < PG_WIDTH / 2)
	loader_max = TOP_LOADER_MAX;
    else
	loader_max = TOP_LOADER_MAX_FAR;
    if ((loader_elements < loader_max
	 && (loader_elements == 0
	     || chrono_remaining_time () > TOP_TIME_LIMIT_MS))
	|| force)
      {
	top_food = food_best (robot_position);
	if (top_food == 0xff)
	    return 0;
	vect_t food_v;
	food_pos (top_food, &food_v);
	/* If loaded an next ball is far away, go unload. */
	if (loader_elements
	    && UTILS_ABS (food_v.x - robot_position.v.x) > 1000)
	  {
	    asserv_set_speed (BOT_MOVE_FAST);
	    move_start_noangle (PG_VECT (2625, 253), 0, 0);
	    return 0;
	  }
	asserv_set_speed (BOT_MOVE_MEDIUM);
	move_start_noangle (food_v, 0, food_shorten (top_food));
	return 1;
      }
    else
      {
	asserv_set_speed (BOT_MOVE_FAST);
	move_start_noangle (PG_VECT (2625, 253), 0, 0);
	return 0;
      }
}

