/* top.c */
/* guybrush - Eurobot 2012 AI. {{{
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
#include "io.h"

#include "playground_2012.h"
#include "asserv.h"

#define FSM_NAME AI
#include "fsm.h"

#include "move.h"
#include "chrono.h"
#include "contact.h"

#include "strat.h"
#include "path.h"

/*
 * Here is the top FSM.  This FSM is suppose to give life to the robot with an
 * impression of intelligence... Well...
 */

FSM_INIT

FSM_STATES (
	    /* Initial state. */
	    TOP_START,
	    /* Going to a collect position above or below a totem. */
	    TOP_TOTEM_GOING,
	    /* Approaching a totem. */
	    TOP_TOTEM_APPROACHING,
	    /* Pushing until full contact. */
	    TOP_TOTEM_PUSHING,
	    /* Going back after totem has been emptied. */
	    TOP_TOTEM_GOING_BACK)

FSM_START_WITH (TOP_START)

/** Top context. */
struct top_t
{
    /** Decision position. */
    vect_t decision_pos;
};

/** Global context. */
struct top_t top;

/** Go collect a totem. */
static void
top_go_totem (void)
{
    position_t pos;
    pos.v = top.decision_pos;
    pos.a = pos.v.y > PG_LENGTH / 2 ? POSITION_A_DEG (-90)
	: POSITION_A_DEG (90);
    move_start (pos, 0);
}

/** Call strat to make a decision, apply it and return the decision to go to
 * the next state. */
static uint8_t
top_decision (void)
{
    uint8_t decision = strat_decision (&top.decision_pos);
    switch (decision)
      {
      case STRAT_DECISION_TOTEM:
	top_go_totem ();
	break;
      default:
	assert (0);
      }
    return decision;
}

FSM_TRANS (TOP_START, init_start_round, TOP_TOTEM_GOING)
{
    strat_init ();
    top_decision ();
    return FSM_NEXT (TOP_START, init_start_round);
}

FSM_TRANS (TOP_TOTEM_GOING, move_success, TOP_TOTEM_APPROACHING)
{
    asserv_move_linearly (PATH_GRID_CLEARANCE_MM - BOT_SIZE_FRONT - 30);
    return FSM_NEXT (TOP_TOTEM_GOING, move_success);
}

FSM_TRANS (TOP_TOTEM_APPROACHING, robot_move_success, TOP_TOTEM_PUSHING)
{
    asserv_push_the_wall (0, -1, -1, -1);
    return FSM_NEXT (TOP_TOTEM_APPROACHING, robot_move_success);
}

FSM_TRANS (TOP_TOTEM_PUSHING, robot_move_success, TOP_TOTEM_GOING_BACK)
{
    asserv_stop_motor ();
    strat_success ();
    move_start_noangle (top.decision_pos, ASSERV_BACKWARD, 0);
    return FSM_NEXT (TOP_TOTEM_PUSHING, robot_move_success);
}

FSM_TRANS (TOP_TOTEM_GOING_BACK, move_success, TOP_TOTEM_GOING)
{
    top_decision ();
    return FSM_NEXT (TOP_TOTEM_GOING_BACK, move_success);
}

