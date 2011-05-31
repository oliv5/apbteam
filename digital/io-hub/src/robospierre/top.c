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

	    TOP_GOING_TO_DROP,
	    TOP_GOING_TO_ELEMENT,
	    /* Waiting clamp has finished its work. */
	    TOP_WAITING_CLAMP)

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

static uint8_t
top_go_element (void)
{
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    ctx.target_element_id = element_best (robot_pos);
    vect_t element_pos = element_get_pos (ctx.target_element_id);
    uint8_t backward = logistic_global.collect_direction == DIRECTION_FORWARD
	? 0 : ASSERV_BACKWARD;
    move_start_noangle (element_pos, backward, 0);
    return 1;
}

static uint8_t
top_go_drop (void)
{
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    uint8_t drop_pos_id = 37;
    vect_t drop_pos = element_get_pos (drop_pos_id);
    uint8_t backward = logistic_global.collect_direction == DIRECTION_FORWARD
	? 0 : ASSERV_BACKWARD;
    move_start_noangle (drop_pos, backward, 0);
    return 0;
}

static uint8_t
top_decision (void)
{
    /* If we can make a tower. */
    if (logistic_global.construct_possible == 1)
	return top_go_drop ();
    else
	return top_go_element ();
}

FSM_TRANS (TOP_GOING_OUT2, robot_move_success,
	   drop, TOP_GOING_TO_DROP,
	   element, TOP_GOING_TO_ELEMENT)
{
    switch (top_decision ())
      {
      default: return FSM_NEXT (TOP_GOING_OUT2, robot_move_success, drop);
      case 1: return FSM_NEXT (TOP_GOING_OUT2, robot_move_success, element);
      }
}

FSM_TRANS (TOP_GOING_TO_DROP, move_success,
	   drop, TOP_GOING_TO_DROP,
	   element, TOP_GOING_TO_ELEMENT)
{
    clamp_drop (logistic_global.collect_direction);
    switch (top_decision ())
      {
      default: return FSM_NEXT (TOP_GOING_TO_DROP, move_success, drop);
      case 1: return FSM_NEXT (TOP_GOING_TO_DROP, move_success, element);
      }
}

FSM_TRANS (TOP_GOING_TO_ELEMENT, move_success,
	   clamp_working, TOP_WAITING_CLAMP,
	   drop, TOP_GOING_TO_DROP,
	   element, TOP_GOING_TO_ELEMENT)
{
    element_taken (ctx.target_element_id, ELEMENT_PAWN);
    if (clamp_working ())
	return FSM_NEXT (TOP_GOING_TO_ELEMENT, move_success, clamp_working);
    switch (top_decision ())
      {
      default: return FSM_NEXT (TOP_GOING_TO_ELEMENT, move_success, drop);
      case 1: return FSM_NEXT (TOP_GOING_TO_ELEMENT, move_success, element);
      }
}

FSM_TRANS (TOP_WAITING_CLAMP, clamp_done,
	   drop, TOP_GOING_TO_DROP,
	   element, TOP_GOING_TO_ELEMENT)
{
    switch (top_decision ())
      {
      default: return FSM_NEXT (TOP_WAITING_CLAMP, clamp_done, drop);
      case 1: return FSM_NEXT (TOP_WAITING_CLAMP, clamp_done, element);
      }
}
