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
#include "move.h"

#include "playground.h"
#include "modules/math/fixed/fixed.h"
#include <math.h>

/* Reset to 0. */
uint8_t top_total_puck_taken = 0;
uint8_t top_puck_inside_bot = 0;

/**
 * Number of unload position.
 */
#define TOP_UNLOAD_POSITION_COUNT 8

#define TOP_UNLOAD_DISTANCE_TO_CENTER (PG_LENGTH / 2 - 600)

/**
 * Structure of a unload position with costs.
 */
typedef struct top_unload_position_t
{
    move_position_t position;
    uint8_t cost;
} top_unload_position_t;

top_unload_position_t top_unload_position[TOP_UNLOAD_POSITION_COUNT];

void
top_init (void)
{
    uint8_t i, cost;
    uint32_t angle;
    for (i = 0; i < TOP_UNLOAD_POSITION_COUNT; i++)
      {
	/* Compute predefined positions. */
	angle = 0x1000000 * i / TOP_UNLOAD_POSITION_COUNT;
	top_unload_position[i].position.x = PG_WIDTH / 2
	    + fixed_mul_f824 (TOP_UNLOAD_DISTANCE_TO_CENTER,
			      fixed_cos_f824 (angle));
	top_unload_position[i].position.y = PG_LENGTH / 2
	    + fixed_mul_f824 (TOP_UNLOAD_DISTANCE_TO_CENTER,
			      fixed_sin_f824 (angle));
	top_unload_position[i].position.a = angle >> 8;
	/* Initialize costs. */
	if (i < TOP_UNLOAD_POSITION_COUNT / 4
	    || i > TOP_UNLOAD_POSITION_COUNT * 3 / 4)
	    cost = bot_color ? 6 : 0;
	else if (i > TOP_UNLOAD_POSITION_COUNT / 4 && i <
		 TOP_UNLOAD_POSITION_COUNT * 3 / 4)
	    cost = bot_color ? 0 : 6;
	else
	    cost = 0;
	top_unload_position[i].cost = cost;
      }
}


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
    else if (restart && current_position != 0)
	current_position--;
    /* Sanity check. */
    if (position)
      {
	/* Fill the position. */
	position->x = PG_X_VALUE_COMPUTING (ground_puck[current_position].x);
	position->y = ground_puck[current_position].y;
	position->a = PG_A_VALUE_COMPUTING (ground_puck[current_position].a *
					    BOT_ANGLE_DEGREE);
      }
    current_position++;
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
	  { { PG_WIDTH - DISTRIBUTOR_PROTECTION, 600, 270 }, { PG_WIDTH - DISTRIBUTOR_PROTECTION, 800, 0 } },
	  /* Third distributor. */
	  { { PG_WIDTH - DISTRIBUTOR_PROTECTION, 1100, 270 }, { PG_WIDTH - DISTRIBUTOR_PROTECTION, 1300, 0 } },
      };
    /* Maximum number of remaining puck by distributor. */
    static uint8_t distributor_puck_count[DISTRIBUTOR_COUNT] = { 5, 5, 5 };
    /* Distributor cost:
     * 7 for a distributor suposed to be not present.
     * 7 for a empty distributor.
     * 1 for a visited distributor.
     * -1 for every puck in distributor.
     */
    static int8_t distributor_cost[DISTRIBUTOR_COUNT];
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
	    /* If empty. */
	    if (distributor_puck_count[current_position] == 0)
		distributor_cost[current_position] += 7;
	    /* Supose the other one is not present. */
	    if (current_position != 0)
		distributor_cost[current_position == 1 ? 2 : 1] += 7;
	  }
	/* Any time, add cost. */
	distributor_cost[current_position]++;
	/* Update taken pucks. */
	previous_total_puck_taken = top_total_puck_taken;
	/* Choose best distributor. */
	uint8_t best = 0;
	uint8_t i;
	for (i = 1; i < DISTRIBUTOR_COUNT; i++)
	  {
	    if (distributor_cost[i] - distributor_puck_count[i] <
		distributor_cost[best] - distributor_puck_count[best])
		best = i;
	  }
	/* Take the best one. */
	current_position = best;

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
    uint8_t i, pos, diff;
    uint8_t dynamic_cost[TOP_UNLOAD_POSITION_COUNT];
    /* Compute angle from table center to current position, and find the
     * nearest position. */
    asserv_position_t current_position;
    asserv_get_position (&current_position);
    pos = (uint8_t) (atan2 (PG_LENGTH / 2 - (int16_t) current_position.y,
			    PG_WIDTH / 2 - (int16_t) current_position.x)
		     * (1.0 / (2 * M_PI)) * TOP_UNLOAD_DISTANCE_TO_CENTER
		     + 1.0 / (TOP_UNLOAD_POSITION_COUNT * 2))
	% TOP_UNLOAD_POSITION_COUNT;
    /* Compute dynamic cost. Nearest position costs 0, near half circle costs
     * 1, far half circle costs 2. */
    for (i = 0; i < TOP_UNLOAD_POSITION_COUNT; i++)
      {
	/* Compute difference between this position and nearest position. */
	diff = (i - pos + TOP_UNLOAD_POSITION_COUNT)
	    % TOP_UNLOAD_POSITION_COUNT;
	if (diff > TOP_UNLOAD_POSITION_COUNT / 2)
	    diff = TOP_UNLOAD_POSITION_COUNT - diff;
	/* Apply cost.  Always add diff to split draws. */
	dynamic_cost[i] = top_unload_position[i].cost
	    * TOP_UNLOAD_POSITION_COUNT;
	if (diff == 0)
	    dynamic_cost[i] += 0 + diff;
	else if (diff <= TOP_UNLOAD_POSITION_COUNT / 4)
	    dynamic_cost[i] += 1 * TOP_UNLOAD_POSITION_COUNT + diff;
	else
	    dynamic_cost[i] += 2 * TOP_UNLOAD_POSITION_COUNT + diff;
      }
    /* Now find the cheapest position. */
    uint8_t best_pos = 0;
    for (i = 1; i < TOP_UNLOAD_POSITION_COUNT; i++)
      {
	if (dynamic_cost[i] < dynamic_cost[best_pos])
	    best_pos = i;
      }
    /* Sanity check. */
    if (position)
      {
	position->x = top_unload_position[best_pos].position.x;
	position->y = top_unload_position[best_pos].position.y;
	position->a = top_unload_position[best_pos].position.a;
      }
    top_unload_position[best_pos].cost++;
}
