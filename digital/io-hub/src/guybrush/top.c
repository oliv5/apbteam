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
#include "modules/utils/utils.h"
#include "io.h"

#include "playground_2012.h"
#include "asserv.h"

#define FSM_NAME AI
#include "fsm.h"

#include "main.h"
#include "move.h"
#include "chrono.h"
#include "contact.h"

#include "strat.h"
#include "path.h"
#include "bottom_clamp.h"

#include "output_defs.h"

/*
 * Here is the top FSM.  This FSM is suppose to give life to the robot with an
 * impression of intelligence... Well...
 */

/** Number of time to try to unblock clamp. */
#define TOP_CLAMP_BLOCKED_THRESHOLD 5

FSM_INIT

FSM_STATES (
	    /* Initial state. */
	    TOP_START,
	    /* Init door, waiting for end of init FSM. */
	    TOP_INIT_DOOR,
	    /* Init done, waiting for rount start. */
	    TOP_INIT,
	    /* Transition state to take a new decision. */
	    TOP_DECISION,
	    /* Unblock clamp and take a new decision. */
	    TOP_CLAMP_ERROR_DECISION,

	    /* Going to a collect position above or below a totem. */
	    TOP_TOTEM_GOING,
	    /* Cleaning: waiting clamp FSM. */
	    TOP_TOTEM_CLEAN_STARTING,
	    /* Cleaning: approaching totem. */
	    TOP_TOTEM_CLEAN_APPROACHING,
	    /* Cleaning; closing clamps. */
	    TOP_TOTEM_CLEAN_CATCH_WAITING,
	    /* Cleaning: going back. */
	    TOP_TOTEM_CLEAN_GOING_BACK,
	    /* Cleaning: loading. */
	    TOP_TOTEM_CLEAN_LOADING,
	    /* Put clamps down. */
	    TOP_TOTEM_CLAMP_DOWNING,
	    /* Approaching a totem. */
	    TOP_TOTEM_APPROACHING,
	    /* Emptying tree. */
	    TOP_TOTEM_EMPTYING,
	    /* Pushing until full contact. */
	    TOP_TOTEM_PUSHING,
	    /* Going back after totem has been emptied. */
	    TOP_TOTEM_GOING_BACK,
	    /* Put clamps up. */
	    TOP_TOTEM_CLAMP_UPPING,
	    /* Release a goldbar blocking the robot. */
	    TOP_TOTEM_ERROR_RELEASE,
	    /* Going back after an error. */
	    TOP_TOTEM_ERROR_GOING_BACK,
	    /* Going back after a clamp error. */
	    TOP_TOTEM_CLAMP_ERROR_GOING_BACK,

	    /* Going to push a bottle. */
	    TOP_BOTTLE_GOING,
	    /* Approaching the button. */
	    TOP_BOTTLE_APPROACHING,
	    /* Push the button. */
	    TOP_BOTTLE_PUSHING,
	    /* Going back after a bottle has been pushed. */
	    TOP_BOTTLE_GOING_BACK,

	    /* Going to an unload position. */
	    TOP_UNLOAD_GOING,
	    /* Unloading, waiting for elements to fall. */
	    TOP_UNLOADING)

FSM_START_WITH (TOP_START)

/** Top context. */
struct top_t
{
    /** Decision position. */
    vect_t decision_pos;
    /** Current distance to totem. */
    int16_t totem_distance;
    /** Close door when out of unloading zone. */
    uint8_t close_door;
};

/** Global context. */
struct top_t top;

void
top_update (void)
{
    /* Close door when out of unloading zone. */
    if (top.close_door)
      {
	position_t robot_pos;
	asserv_get_position (&robot_pos);
	if (robot_pos.v.x > PG_HOLD_NORTH_X + BOT_SIZE_BACK
	    && robot_pos.v.x < PG_MIRROR_X (PG_HOLD_NORTH_X + BOT_SIZE_BACK))
	  {
	    IO_CLR (OUTPUT_DOOR_OPEN);
	    IO_SET (OUTPUT_DOOR_CLOSE);
	    top.close_door = 0;
	  }
      }
    /* If door is closed, cut air to save power. */
    if (!IO_GET (CONTACT_DOOR_CLOSE))
      {
	IO_CLR (OUTPUT_DOOR_CLOSE);
      }
}

/** Go collect a totem. */
static void
top_go_totem (void)
{
    position_t pos;
    pos.v = top.decision_pos;
    pos.a = pos.v.y > PG_LENGTH / 2 ? POSITION_A_DEG (-90)
	: POSITION_A_DEG (90);
    move_start (pos, 0);
    top.totem_distance = UTILS_ABS (pos.v.y - PG_LENGTH / 2)
	- PG_TOTEM_WIDTH_MM / 2;
}

/** Go push a bottle button. */
static void
top_go_bottle (void)
{
    position_t pos;
    pos.v = top.decision_pos;
    pos.a = POSITION_A_DEG (90);
    move_start (pos, 0);
}

/** Go unload. */
static void
top_go_unload (void)
{
    position_t pos;
    pos.v = top.decision_pos;
    if (pos.v.y > PG_LENGTH - PG_CAPTAIN_ROOM_LENGTH_MM)
	pos.a = PG_A_DEG (0);
    else
	pos.a = PG_A_DEG (70);
    move_start (pos, 0);
}

/** Call strat to make a decision, apply it and return the decision to go to
 * the next state. */
static uint8_t
top_decision (void)
{
    asserv_set_speed (BOT_SPEED_NORMAL);
    uint8_t decision = strat_decision (&top.decision_pos);
    switch (decision)
      {
      case STRAT_DECISION_TOTEM:
	top_go_totem ();
	break;
      case STRAT_DECISION_BOTTLE:
	top_go_bottle ();
	break;
      case STRAT_DECISION_UNLOAD:
	top_go_unload ();
	break;
      default:
	assert (0);
      }
    return decision;
}

FSM_TRANS (TOP_START, init_actuators, TOP_INIT_DOOR)
{
    main_demo = !IO_GET (CONTACT_STRAT);
    IO_SET (OUTPUT_DOOR_OPEN);
    IO_CLR (OUTPUT_DOOR_CLOSE);
    return FSM_NEXT (TOP_START, init_actuators);
}

FSM_TRANS (TOP_INIT_DOOR, init_done, TOP_INIT)
{
    IO_CLR (OUTPUT_DOOR_OPEN);
    IO_SET (OUTPUT_DOOR_CLOSE);
    strat_init ();
    strat_prepare ();
    return FSM_NEXT (TOP_INIT_DOOR, init_done);
}

FSM_TRANS (TOP_INIT, init_start_round, TOP_DECISION)
{
    main_usdist = 1;
    return FSM_NEXT (TOP_INIT, init_start_round);
}

FSM_TRANS_TIMEOUT (TOP_DECISION, 1,
		   clamp_blocked, TOP_CLAMP_ERROR_DECISION,
		   totem, TOP_TOTEM_GOING,
		   bottle, TOP_BOTTLE_GOING,
		   unload, TOP_UNLOAD_GOING)
{
    if (FSM_HANDLE (AI, clamp_unblock))
      {
	return FSM_NEXT_TIMEOUT (TOP_DECISION, clamp_blocked);
      }
    switch (top_decision ())
      {
      default:
	assert (0);
      case STRAT_DECISION_TOTEM:
	return FSM_NEXT_TIMEOUT (TOP_DECISION, totem);
      case STRAT_DECISION_BOTTLE:
	return FSM_NEXT_TIMEOUT (TOP_DECISION, bottle);
      case STRAT_DECISION_UNLOAD:
	return FSM_NEXT_TIMEOUT (TOP_DECISION, unload);
      }
}

FSM_TRANS (TOP_CLAMP_ERROR_DECISION, clamps_ready, TOP_DECISION)
{
    return FSM_NEXT (TOP_CLAMP_ERROR_DECISION, clamps_ready);
}

FSM_TRANS (TOP_CLAMP_ERROR_DECISION, clamp_blocked,
	   unblock, TOP_CLAMP_ERROR_DECISION,
	   dead, TOP_DECISION)
{
    if (clamp_read_blocked_cpt () < TOP_CLAMP_BLOCKED_THRESHOLD)
      {
	FSM_HANDLE (AI, clamp_unblock);
	return FSM_NEXT (TOP_CLAMP_ERROR_DECISION, clamp_blocked, unblock);
      }
    else
      {
	strat_clamp_dead ();
	FSM_HANDLE (AI, clamp_is_dead);
	return FSM_NEXT (TOP_CLAMP_ERROR_DECISION, clamp_blocked, dead);
      }
}

/** TOTEM */

FSM_TRANS (TOP_TOTEM_GOING, move_success, TOP_TOTEM_CLEAN_STARTING)
{
    clamp_request (FSM_EVENT (AI, clean_start));
    return FSM_NEXT (TOP_TOTEM_GOING, move_success);
}

FSM_TRANS (TOP_TOTEM_CLEAN_STARTING, clamps_ready, TOP_TOTEM_CLEAN_APPROACHING)
{
    int16_t move = top.totem_distance - BOT_SIZE_LOWER_CLAMP_FRONT - 140;
    top.totem_distance -= move;
    asserv_set_speed (BOT_SPEED_APPROACH);
    asserv_move_linearly (move);
    return FSM_NEXT (TOP_TOTEM_CLEAN_STARTING, clamps_ready);
}

FSM_TRANS (TOP_TOTEM_CLEAN_APPROACHING, robot_move_success,
	   TOP_TOTEM_CLEAN_CATCH_WAITING)
{
    FSM_HANDLE (AI, clean_catch);
    return FSM_NEXT (TOP_TOTEM_CLEAN_APPROACHING, robot_move_success);
}

FSM_TRANS (TOP_TOTEM_CLEAN_APPROACHING, robot_move_failure,
	   TOP_TOTEM_CLEAN_CATCH_WAITING)
{
    /* Must be blocked on a CD, catch and continue. */
    FSM_HANDLE (AI, clean_catch);
    return FSM_NEXT (TOP_TOTEM_CLEAN_APPROACHING, robot_move_failure);
}

FSM_TRANS (TOP_TOTEM_CLEAN_CATCH_WAITING, clamps_ready,
	   TOP_TOTEM_CLEAN_GOING_BACK)
{
    int16_t move = top.totem_distance - BOT_SIZE_LOADING_FRONT - 30;
    top.totem_distance -= move;
    asserv_move_linearly (move);
    return FSM_NEXT (TOP_TOTEM_CLEAN_CATCH_WAITING, clamps_ready);
}

FSM_TRANS (TOP_TOTEM_CLEAN_GOING_BACK, robot_move_success,
	   TOP_TOTEM_CLEAN_LOADING)
{
    FSM_HANDLE (AI, clean_load);
    return FSM_NEXT (TOP_TOTEM_CLEAN_GOING_BACK, robot_move_success);
}

FSM_TRANS (TOP_TOTEM_CLEAN_GOING_BACK, robot_move_failure,
	   TOP_TOTEM_CLEAN_LOADING)
{
    FSM_HANDLE (AI, clean_load);
    return FSM_NEXT (TOP_TOTEM_CLEAN_GOING_BACK, robot_move_failure);
}

FSM_TRANS (TOP_TOTEM_CLEAN_LOADING, clamps_ready, TOP_TOTEM_CLAMP_DOWNING)
{
    clamp_request (FSM_EVENT (AI, tree_detected));
    return FSM_NEXT (TOP_TOTEM_CLEAN_LOADING, clamps_ready);
}

FSM_TRANS (TOP_TOTEM_CLAMP_DOWNING, clamps_ready, TOP_TOTEM_APPROACHING)
{
    int16_t move = top.totem_distance - BOT_SIZE_FRONT - 30;
    top.totem_distance -= move;
    asserv_move_linearly (move);
    return FSM_NEXT (TOP_TOTEM_CLAMP_DOWNING, clamps_ready);
}

FSM_TRANS (TOP_TOTEM_APPROACHING, robot_move_success, TOP_TOTEM_PUSHING)
{
    asserv_push_the_wall (0, -1, -1, -1);
    return FSM_NEXT (TOP_TOTEM_APPROACHING, robot_move_success);
}

FSM_TRANS (TOP_TOTEM_PUSHING, robot_move_success, TOP_TOTEM_EMPTYING)
{
    asserv_stop_motor ();
    FSM_HANDLE (AI, empty_tree);
    return FSM_NEXT (TOP_TOTEM_PUSHING, robot_move_success);
}

FSM_TRANS (TOP_TOTEM_EMPTYING, clamps_ready, TOP_TOTEM_GOING_BACK)
{
    strat_success ();
    move_start_noangle (top.decision_pos, ASSERV_BACKWARD, 0);
    return FSM_NEXT (TOP_TOTEM_EMPTYING, clamps_ready);
}

FSM_TRANS (TOP_TOTEM_GOING_BACK, move_success, TOP_TOTEM_CLAMP_UPPING)
{
    FSM_HANDLE (AI, robot_is_back);
    return FSM_NEXT (TOP_TOTEM_GOING_BACK, move_success);
}

FSM_TRANS (TOP_TOTEM_CLAMP_UPPING, clamps_ready, TOP_DECISION)
{
    return FSM_NEXT (TOP_TOTEM_CLAMP_UPPING, clamps_ready);
}

/** TOTEM failures. */

FSM_TRANS (TOP_TOTEM_GOING, move_failure, TOP_DECISION)
{
    strat_failure ();
    return FSM_NEXT (TOP_TOTEM_GOING, move_failure);
}

FSM_TRANS (TOP_TOTEM_CLEAN_STARTING, clamp_blocked, TOP_CLAMP_ERROR_DECISION)
{
    strat_failure ();
    FSM_HANDLE (AI, clamp_unblock);
    return FSM_NEXT (TOP_TOTEM_CLEAN_STARTING, clamp_blocked);
}

FSM_TRANS (TOP_TOTEM_CLEAN_CATCH_WAITING, clamp_blocked,
	   TOP_TOTEM_CLAMP_ERROR_GOING_BACK)
{
    strat_failure ();
    move_start_noangle (top.decision_pos, ASSERV_BACKWARD, 0);
    return FSM_NEXT (TOP_TOTEM_CLEAN_CATCH_WAITING, clamp_blocked);
}

FSM_TRANS (TOP_TOTEM_CLEAN_LOADING, clamp_blocked, TOP_CLAMP_ERROR_DECISION)
{
    strat_failure ();
    FSM_HANDLE (AI, clamp_unblock);
    return FSM_NEXT (TOP_TOTEM_CLEAN_LOADING, clamp_blocked);
}

FSM_TRANS (TOP_TOTEM_CLAMP_DOWNING, clamp_blocked, TOP_CLAMP_ERROR_DECISION)
{
    strat_failure ();
    FSM_HANDLE (AI, clamp_unblock);
    return FSM_NEXT (TOP_TOTEM_CLAMP_DOWNING, clamp_blocked);
}

FSM_TRANS (TOP_TOTEM_APPROACHING, robot_move_failure,
	   TOP_TOTEM_ERROR_GOING_BACK)
{
    strat_giveup ();
    move_start_noangle (top.decision_pos, ASSERV_BACKWARD, 0);
    return FSM_NEXT (TOP_TOTEM_APPROACHING, robot_move_failure);
}

FSM_TRANS (TOP_TOTEM_PUSHING, robot_move_failure,
	   TOP_TOTEM_ERROR_GOING_BACK)
{
    strat_giveup ();
    move_start_noangle (top.decision_pos, ASSERV_BACKWARD, 0);
    return FSM_NEXT (TOP_TOTEM_PUSHING, robot_move_failure);
}

FSM_TRANS (TOP_TOTEM_EMPTYING, clamp_blocked,
	   TOP_TOTEM_CLAMP_ERROR_GOING_BACK)
{
    strat_giveup ();
    move_start_noangle (top.decision_pos, ASSERV_BACKWARD, 0);
    return FSM_NEXT (TOP_TOTEM_EMPTYING, clamp_blocked);
}

FSM_TRANS (TOP_TOTEM_GOING_BACK, move_failure, TOP_TOTEM_ERROR_RELEASE)
{
    strat_giveup ();
    FSM_HANDLE (AI, stop_tree_approach);
    return FSM_NEXT (TOP_TOTEM_GOING_BACK, move_failure);
}

FSM_TRANS (TOP_TOTEM_CLAMP_UPPING, clamp_blocked, TOP_CLAMP_ERROR_DECISION)
{
    strat_giveup ();
    FSM_HANDLE (AI, clamp_unblock);
    return FSM_NEXT (TOP_TOTEM_CLAMP_UPPING, clamp_blocked);
}

FSM_TRANS (TOP_TOTEM_ERROR_RELEASE, clamps_ready, TOP_TOTEM_ERROR_GOING_BACK)
{
    move_start_noangle (top.decision_pos, ASSERV_BACKWARD, 0);
    return FSM_NEXT (TOP_TOTEM_ERROR_RELEASE, clamps_ready);
}

FSM_TRANS (TOP_TOTEM_ERROR_GOING_BACK, move_success, TOP_TOTEM_CLAMP_UPPING)
{
    FSM_HANDLE (AI, stop_tree_approach);
    return FSM_NEXT (TOP_TOTEM_ERROR_GOING_BACK, move_success);
}

FSM_TRANS (TOP_TOTEM_ERROR_GOING_BACK, move_failure, TOP_TOTEM_CLAMP_UPPING)
{
    FSM_HANDLE (AI, stop_tree_approach);
    return FSM_NEXT (TOP_TOTEM_ERROR_GOING_BACK, move_failure);
}

FSM_TRANS (TOP_TOTEM_CLAMP_ERROR_GOING_BACK, move_success,
	   TOP_CLAMP_ERROR_DECISION)
{
    FSM_HANDLE (AI, clamp_unblock);
    return FSM_NEXT (TOP_TOTEM_CLAMP_ERROR_GOING_BACK, move_success);
}

FSM_TRANS (TOP_TOTEM_CLAMP_ERROR_GOING_BACK, move_failure,
	   TOP_CLAMP_ERROR_DECISION)
{
    FSM_HANDLE (AI, clamp_unblock);
    return FSM_NEXT (TOP_TOTEM_CLAMP_ERROR_GOING_BACK, move_failure);
}

/** BOTTLE */

FSM_TRANS (TOP_BOTTLE_GOING, move_success, TOP_BOTTLE_APPROACHING)
{
    int16_t move = top.decision_pos.y - BOT_SIZE_BACK - 22 - 30;
    asserv_set_speed (BOT_SPEED_APPROACH);
    asserv_move_linearly (-move);
    return FSM_NEXT (TOP_BOTTLE_GOING, move_success);
}

FSM_TRANS (TOP_BOTTLE_APPROACHING, robot_move_success, TOP_BOTTLE_PUSHING)
{
    asserv_push_the_wall (ASSERV_BACKWARD, -1, -1, -1);
    return FSM_NEXT (TOP_BOTTLE_APPROACHING, robot_move_success);
}

FSM_TRANS (TOP_BOTTLE_PUSHING, robot_move_success, TOP_BOTTLE_GOING_BACK)
{
    asserv_stop_motor ();
    strat_success ();
    move_start_noangle (top.decision_pos, 0, 0);
    return FSM_NEXT (TOP_BOTTLE_PUSHING, robot_move_success);
}

FSM_TRANS (TOP_BOTTLE_PUSHING, robot_move_failure, TOP_BOTTLE_GOING_BACK)
{
    /* OK, ignore failure. */
    asserv_stop_motor ();
    strat_success ();
    move_start_noangle (top.decision_pos, 0, 0);
    return FSM_NEXT (TOP_BOTTLE_PUSHING, robot_move_failure);
}

FSM_TRANS (TOP_BOTTLE_GOING_BACK, move_success, TOP_DECISION)
{
    return FSM_NEXT (TOP_BOTTLE_GOING_BACK, move_success);
}

FSM_TRANS (TOP_BOTTLE_GOING_BACK, move_failure, TOP_DECISION)
{
    /* Ignore and continue. */
    return FSM_NEXT (TOP_BOTTLE_GOING_BACK, move_failure);
}

/** BOTTLE failures. */

FSM_TRANS (TOP_BOTTLE_GOING, move_failure, TOP_DECISION)
{
    strat_failure ();
    return FSM_NEXT (TOP_BOTTLE_GOING, move_failure);
}

FSM_TRANS (TOP_BOTTLE_APPROACHING, robot_move_failure,
	   try_anyway, TOP_BOTTLE_PUSHING,
	   decision, TOP_DECISION)
{
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    /* Try to continue anyway? */
    if (robot_pos.v.y < BOT_SIZE_BACK + 100)
      {
	asserv_push_the_wall (ASSERV_BACKWARD, -1, -1, -1);
	return FSM_NEXT (TOP_BOTTLE_APPROACHING, robot_move_failure,
			 try_anyway);
      }
    else
      {
	strat_failure ();
	return FSM_NEXT (TOP_BOTTLE_APPROACHING, robot_move_failure,
			 decision);
      }
}

/** UNLOAD */

FSM_TRANS (TOP_UNLOAD_GOING, move_success, TOP_UNLOADING)
{
    IO_SET (OUTPUT_DOOR_OPEN);
    IO_CLR (OUTPUT_DOOR_CLOSE);
    return FSM_NEXT (TOP_UNLOAD_GOING, move_success);
}

FSM_TRANS_TIMEOUT (TOP_UNLOADING, 250, TOP_DECISION)
{
    strat_success ();
    top.close_door = 1;
    return FSM_NEXT_TIMEOUT (TOP_UNLOADING);
}

/** UNLOAD failures. */

FSM_TRANS (TOP_UNLOAD_GOING, move_failure, TOP_DECISION)
{
    strat_failure ();
    return FSM_NEXT (TOP_UNLOAD_GOING, move_failure);
}

