/* top.c */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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

#include "playground.h"
#include "asserv.h"

#define FSM_NAME AI
#include "fsm.h"

#include "logistic.h"
#include "move.h"

/*
 * Here is the top FSM.  This FSM is suppose to give life to the robot with an
 * impression of intelligence... Well...
 */

FSM_INIT

FSM_STATES (
	    /* Initial state. */
	    TOP_START,
	    /* Going out of start area. */
	    TOP_GOING_OUT1,
	    /* Going out, first pawn emplacement. */
	    TOP_GOING_OUT2,

	    TOP_GOING_TO_ELEMENT,
	    TOP_GOING_TO_GREEN_POS,
	    TOP_GOING_TO_GREEN_ELEMENT,
	    TOP_GOING_FROM_GREEN_ELEMENT)

FSM_START_WITH (TOP_START)

/** Top context. */
struct top_t
{
    /** Target element. */
    uint8_t target_element_id;
};

/** Global context. */
struct top_t top_global;
#define ctx top_global

FSM_TRANS (TOP_START, init_start_round, TOP_GOING_OUT1)
{
    element_init ();
    asserv_goto (PG_X (400 + 100), PG_Y (PG_LENGTH - 200), 0);
    return FSM_NEXT (TOP_START, init_start_round);
}

FSM_TRANS (TOP_GOING_OUT1, robot_move_success, TOP_GOING_OUT2)
{
    asserv_goto (PG_X (1500 - 2 * 350), PG_Y (PG_LENGTH - 350), 0);
    return FSM_NEXT (TOP_GOING_OUT1, robot_move_success);
}

uint8_t
top_go_element (void)
{
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    ctx.target_element_id = element_best (robot_pos);
    position_t element_pos = element_get_pos (ctx.target_element_id);
    if (element_pos.a != 0xffff)
      {
	/* Green zone element. */
	move_start (element_pos, 0);
	return 2;
      }
    else
      {
	/* Regular element. */
	move_start_noangle (element_pos.v, 0, 0);
	return 1;
      }
}

FSM_TRANS (TOP_GOING_OUT2, robot_move_success,
	   element, TOP_GOING_TO_ELEMENT,
	   green_element, TOP_GOING_TO_GREEN_POS)
{
    if (top_go_element () == 1)
	return FSM_NEXT (TOP_GOING_OUT2, robot_move_success, element);
    else
	return FSM_NEXT (TOP_GOING_OUT2, robot_move_success, green_element);
}

FSM_TRANS (TOP_GOING_TO_ELEMENT, move_success,
	   element, TOP_GOING_TO_ELEMENT,
	   green_element, TOP_GOING_TO_GREEN_POS)
{
    if (top_go_element () == 1)
	return FSM_NEXT (TOP_GOING_TO_ELEMENT, move_success, element);
    else
	return FSM_NEXT (TOP_GOING_TO_ELEMENT, move_success, green_element);
}

FSM_TRANS (TOP_GOING_TO_GREEN_POS, move_success, TOP_GOING_TO_GREEN_ELEMENT)
{
    asserv_move_linearly (BOT_GREEN_ELEMENT_MOVE_DISTANCE_MM);
    return FSM_NEXT (TOP_GOING_TO_GREEN_POS, move_success);
}

FSM_TRANS (TOP_GOING_TO_GREEN_ELEMENT, robot_move_success,
	   TOP_GOING_FROM_GREEN_ELEMENT)
{
    element_taken (ctx.target_element_id, ELEMENT_PAWN);
    asserv_move_linearly (-BOT_GREEN_ELEMENT_MOVE_DISTANCE_MM);
    return FSM_NEXT (TOP_GOING_TO_GREEN_ELEMENT, robot_move_success);
}

FSM_TRANS (TOP_GOING_FROM_GREEN_ELEMENT, robot_move_success,
	   element, TOP_GOING_TO_ELEMENT,
	   green_element, TOP_GOING_TO_GREEN_POS)
{
    if (top_go_element () == 1)
	return FSM_NEXT (TOP_GOING_FROM_GREEN_ELEMENT, robot_move_success,
			 element);
    else
	return FSM_NEXT (TOP_GOING_FROM_GREEN_ELEMENT, robot_move_success,
			 green_element);
}

