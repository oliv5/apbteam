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

/** Maximum elements to load before unloading. */
#define TOP_LOADER_MAX 3
/** Time to reserve for unloading at end of round. */
#define TOP_TIME_LIMIT_MS 20000ll

void
top_init (void)
{
}

uint8_t
top_collect (uint8_t force)
{
    if ((loader_elements < TOP_LOADER_MAX
	 && (loader_elements == 0
	     || chrono_remaining_time () > TOP_TIME_LIMIT_MS))
	|| force)
      {
	position_t robot_position;
	asserv_get_position (&robot_position);
	uint8_t food = food_best (robot_position);
	if (food == 0xff)
	    return 0;
	vect_t food_v;
	food_pos (food, &food_v);
	move_start_noangle (food_v, 0, food_shorten (food));
	return 1;
      }
    else
      {
	move_start_noangle (PG_VECT (2625, 253), 0, 0);
	return 0;
      }
}

