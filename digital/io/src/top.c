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
#include "top.h"

#include "playground.h"

/* Reset to 0. */
uint8_t top_total_puck_taken = 0;
uint8_t top_puck_inside_bot = 0;

uint8_t
top_get_next_position_to_get_puck_on_the_ground (asserv_position_t *position,
						 uint8_t restart)
{
    /* All positions, in order. */
    static const asserv_position_t ground_puck[] =
      {
	  /* Position: 1 */
	  { 600, 1525,  270 },
	  /* Position: 4 */
	  { 600, 925,  270 },
	  /* Position: 6 */
	  { 1100, 925,  0 },
	  /* Position: 9 */
	  { 1100, 1525,  90 },
	  /* Position: 10 */
	  { 850, 1525,  180 },
	  /* Position: 10 */
	  { 850, 925,  270 },
      };
    static uint8_t current_position = 0;

    if (current_position >= (sizeof (ground_puck)
			    / sizeof (asserv_position_t)))
	/* No more position. */
	return 0;
    /* Sanity check. */
    if (position)
      {
	/* Fill the position. */
	position->x = PG_X_VALUE_COMPUTING (ground_puck[current_position].x);
	position->y = ground_puck[current_position].y;
	position->a = PG_A_VALUE_COMPUTING (ground_puck[current_position].a *
					    BOT_ANGLE_DEGREE);
	/* Go to next position only if we are restarting the procedure. */
	if (!restart)
	    current_position++;
      }
    return 1;
}

void
top_get_next_position_to_get_distributor (asserv_position_t *clean_position,
					  asserv_position_t *front_position)
{
    /* Number of distributor. */
#define DISTRIBUTOR_COUNT 3
#define DISTRIBUTOR_PROTECTION 360
    /* Table of distributor, first index is clean, second is front. */
    static const asserv_position_t distributor[DISTRIBUTOR_COUNT][2] =
      {
	  /* First distributor. */
	  { { 2711, 0 + DISTRIBUTOR_PROTECTION, 270 }, { 2711, DISTRIBUTOR_PROTECTION, 270 } },
	  /* Second distributor. */
	  { { PG_WIDTH - DISTRIBUTOR_PROTECTION, 600, 90 }, { PG_WIDTH - DISTRIBUTOR_PROTECTION, 800, 0 } },
	  /* Third distributor. */
	  { { PG_WIDTH - DISTRIBUTOR_PROTECTION, 1100, 90 }, { PG_WIDTH - DISTRIBUTOR_PROTECTION, 1300, 0 } },
      };
    /* Maximum number of remaining puck by distributor. */
    static uint8_t distributor_puck_count[DISTRIBUTOR_COUNT] = { 5, 5, 5 };
    /* Current distributor. */
    static uint8_t current_position = DISTRIBUTOR_COUNT - 1;
    /* Keep track of how many pucks we have get. This variable will be used to
     * know how many puck were taken for each distributor. */
    static uint8_t previous_total_puck_taken = 0;
    if (!previous_total_puck_taken)
	previous_total_puck_taken = top_total_puck_taken;

    /* Sanity check. */
    if (clean_position && front_position)
      {
	/* Compute taken pucks since last time. */
	uint8_t puck_count = top_total_puck_taken - previous_total_puck_taken;
	/* If some pucks were taken. */
	if (puck_count != 0)
	  {
	    /* Remove them. */
	    distributor_puck_count[current_position] -= (top_total_puck_taken -
							 previous_total_puck_taken);
	  }
	/* If no puck taken or distributor empty. */
	if (puck_count == 0 || distributor_puck_count[current_position] == 0)
	  {
	    /* Next distributor. */
	    current_position++;
	    current_position %= 3;
	  }
	/* Update taken pucks. */
	previous_total_puck_taken = top_total_puck_taken;

	/* Fill the clean position. */
	clean_position->x = PG_X_VALUE_COMPUTING
	    (distributor[current_position][0].x);
	clean_position->y = distributor[current_position][0].y;
	clean_position->a = PG_A_VALUE_COMPUTING
	    (distributor[current_position][0].a * BOT_ANGLE_DEGREE);
	/* Fill the distributor position. */
	front_position->x = PG_X_VALUE_COMPUTING
	    (distributor[current_position][1].x);
	front_position->y = distributor[current_position][1].y;
	front_position->a = PG_A_VALUE_COMPUTING
	    (distributor[current_position][1].a * BOT_ANGLE_DEGREE);
      }
}

void
top_get_next_position_to_unload_puck (asserv_position_t *position)
{
    /* TODO: enahnce. */
    static const asserv_position_t unload[] =
      {
	  { 1500, 600, 270 },
      };
    /* Sanity check. */
    if (position)
      {
	position->x = PG_X_VALUE_COMPUTING (unload->x);
	position->y = unload->y;
	position->a = PG_A_VALUE_COMPUTING (unload->a * BOT_ANGLE_DEGREE);
      }
}
