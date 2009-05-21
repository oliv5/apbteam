/* top_cb.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Dufour Jérémy
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
 * }}}
 * Main FSM calling other FSM.
 */
#include "common.h"
#include "fsm.h"
#include "top_cb.h"
#include "top.h"
#include "move.h"
#include "chrono.h"
#include "playground.h"
#include "asserv.h"
#include "cylinder.h"
#include "elevator.h"

/**
 * Internal data.
 */
asserv_position_t front_position;
uint8_t remember_distributor = 0;

/**
 * Time limit.
 */
#define TOP_TIME_LIMIT 25000

/*
 * IDLE =start=>
 *  => WAIT_INIT_TO_FINISH
 *   nothing to do.
 */
fsm_branch_t
top__IDLE__start (void)
{
    return top_next (IDLE, start);
}

/*
 * WAIT_INIT_TO_FINISH =init_match_is_started=>
 *  => GET_PUCK_FROM_THE_GROUND
 *   the match start, try to get some puck from the ground.
 */
fsm_branch_t
top__WAIT_INIT_TO_FINISH__init_match_is_started (void)
{
    /* Get next position. */
    asserv_position_t position;
    top_get_next_position_to_get_puck_on_the_ground (&position, 0);
    /* Go there. */
    move_start (position, 0);
    return top_next (WAIT_INIT_TO_FINISH, init_match_is_started);
}

/*
 * GET_PUCK_FROM_THE_GROUND =move_fsm_succeed=>
 * already_six_pucks_or_no_next_position => GET_PUCK_FROM_DISTRIBUTOR
 *   get the next distributor position and launch move FSM to go there.
 * next_position_exists => GET_PUCK_FROM_THE_GROUND
 *   go to the next position using move FSM.
 */
fsm_branch_t
top__GET_PUCK_FROM_THE_GROUND__move_fsm_succeed (void)
{
    /* Get next position. */
    asserv_position_t position;
    if (!top_get_next_position_to_get_puck_on_the_ground (&position, 0)
	|| top_total_puck_taken >= 6)
      {
	/* Ensure cylinder is close. */
	cylinder_close_order = 1;
	/* Go to distributor. */
	top_get_next_position_to_get_distributor (&position, &front_position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (GET_PUCK_FROM_THE_GROUND, move_fsm_succeed, already_six_pucks_or_no_next_position);
      }
    else
      {
	/* Go there. */
	move_start (position, 0);
	return top_next_branch (GET_PUCK_FROM_THE_GROUND, move_fsm_succeed, next_position_exists);
      }
}

/*
 * GET_PUCK_FROM_THE_GROUND =move_fsm_failed=>
 *  => GET_PUCK_FROM_DISTRIBUTOR
 *   we have failed to do a move, go the distributor (get next distributor and go
 *   there with move FSM).
 */
fsm_branch_t
top__GET_PUCK_FROM_THE_GROUND__move_fsm_failed (void)
{
    /* Get next position. */
    asserv_position_t position;
    /* Close the cylinder. */
    cylinder_close_order = 1;
    /* Go to distributor. */
    top_get_next_position_to_get_distributor (&position, &front_position);
    /* Go there. */
    move_start (position, ASSERV_BACKWARD);
    return top_next (GET_PUCK_FROM_THE_GROUND, move_fsm_failed);
}

/*
 * GET_PUCK_FROM_THE_GROUND =bot_is_full_of_pucks=>
 *  => STOP_TO_GO_TO_UNLOAD_AREA
 *   stop move FSM.
 */
fsm_branch_t
top__GET_PUCK_FROM_THE_GROUND__bot_is_full_of_pucks (void)
{
    /* Stop move FSM. */
    move_stop ();
    return top_next (GET_PUCK_FROM_THE_GROUND, bot_is_full_of_pucks);
}

/*
 * GET_PUCK_FROM_THE_GROUND =state_timeout=>
 *  => STOP_TO_GET_PUCK_FROM_DISTRIBUTOR
 *   close cylinder.
 *   too much time lost to get puck from the ground, stop move FSM.
 */
fsm_branch_t
top__GET_PUCK_FROM_THE_GROUND__state_timeout (void)
{
    /* Stop move FSM. */
    move_stop ();
    /* Close cylinder. */
    cylinder_close_order = 1;
    return top_next (GET_PUCK_FROM_THE_GROUND, state_timeout);
}

/*
 * GET_PUCK_FROM_DISTRIBUTOR =move_fsm_succeed=>
 *  => CLEAN_FRONT_OF_DISTRIBUTOR
 *   really go to the front of the distributor (it clean front area).
 */
fsm_branch_t
top__GET_PUCK_FROM_DISTRIBUTOR__move_fsm_succeed (void)
{
    /* Open cylinder. */
    cylinder_close_order = 0;
    /* Go in the front of the distributor. */
    move_start (front_position, ASSERV_REVERT_OK);
    return top_next (GET_PUCK_FROM_DISTRIBUTOR, move_fsm_succeed);
}

/*
 * GET_PUCK_FROM_DISTRIBUTOR =move_fsm_failed=>
 * no_puck_or_still_time => GET_PUCK_FROM_DISTRIBUTOR
 *   get the next distributor position and launch move FSM to go there.
 * some_pucks_and_no_more_time => GO_TO_UNLOAD_AREA
 *   compute an unload area.
 *   ask move FSM to go there.
 */
fsm_branch_t
top__GET_PUCK_FROM_DISTRIBUTOR__move_fsm_failed (void)
{
    if (!top_puck_inside_bot || chrono_remaining_time () > TOP_TIME_LIMIT)
      {
	/* Get next position. */
	asserv_position_t position;
	/* Go to distributor. */
	top_get_next_position_to_get_distributor (&position, &front_position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (GET_PUCK_FROM_DISTRIBUTOR, move_fsm_failed, no_puck_or_still_time);
      }
    else
      {
	/* Ensure cylinder is closed. */
	cylinder_close_order = 1;
	/* Flush cylinder. */
	cylinder_flush_order = 1;
	asserv_position_t position;
	/* Go to unload area. */
	top_get_next_position_to_unload_puck (&position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (GET_PUCK_FROM_DISTRIBUTOR, move_fsm_failed, some_pucks_and_no_more_time);
      }
}

/*
 * STOP_TO_GO_TO_UNLOAD_AREA =move_fsm_stopped=>
 *  => GO_TO_UNLOAD_AREA
 *   compute an unload area.
 *   ask move FSM to go there using backward move only.
 */
fsm_branch_t
top__STOP_TO_GO_TO_UNLOAD_AREA__move_fsm_stopped (void)
{
    /* Ensure cylinder is closed. */
    cylinder_close_order = 1;
    /* Flush cylinder. */
    cylinder_flush_order = 1;
    asserv_position_t position;
    /* Go to unload area. */
    top_get_next_position_to_unload_puck (&position);
    /* Go there. */
    move_start (position, ASSERV_BACKWARD);
    return top_next (STOP_TO_GO_TO_UNLOAD_AREA, move_fsm_stopped);
}

/*
 * STOP_TO_GO_TO_UNLOAD_AREA =move_fsm_succeed=>
 *  => GO_TO_UNLOAD_AREA
 *   do the same as bot_move_stopped.
 */
fsm_branch_t
top__STOP_TO_GO_TO_UNLOAD_AREA__move_fsm_succeed (void)
{
    /* Yerk. */
    top__STOP_TO_GO_TO_UNLOAD_AREA__move_fsm_stopped ();
    return top_next (STOP_TO_GO_TO_UNLOAD_AREA, move_fsm_succeed);
}

/*
 * STOP_TO_GO_TO_UNLOAD_AREA =bot_move_failed=>
 *  => GO_TO_UNLOAD_AREA
 *   do the same as bot_move_stopped.
 */
fsm_branch_t
top__STOP_TO_GO_TO_UNLOAD_AREA__bot_move_failed (void)
{
    /* Yerk. */
    top__STOP_TO_GO_TO_UNLOAD_AREA__move_fsm_stopped ();
    return top_next (STOP_TO_GO_TO_UNLOAD_AREA, bot_move_failed);
}

/*
 * STOP_TO_GET_PUCK_FROM_DISTRIBUTOR =move_fsm_stopped=>
 *  => GET_PUCK_FROM_DISTRIBUTOR
 *   get next distributor and go there.
 */
fsm_branch_t
top__STOP_TO_GET_PUCK_FROM_DISTRIBUTOR__move_fsm_stopped (void)
{
    /* Ensure cylinder is closed. */
    cylinder_close_order = 1;
    /* Get next position. */
    asserv_position_t position;
    /* Go to distributor. */
    top_get_next_position_to_get_distributor (&position, &front_position);
    /* Go there. */
    move_start (position, ASSERV_BACKWARD);
    return top_next (STOP_TO_GET_PUCK_FROM_DISTRIBUTOR, move_fsm_stopped);
}

/*
 * GO_TO_UNLOAD_AREA =move_fsm_succeed=>
 *  => PREPARE_ELEVATOR
 *   prepare the elevator.
 */
fsm_branch_t
top__GO_TO_UNLOAD_AREA__move_fsm_succeed (void)
{
    /* Prepare elevator. */
    elvt_order = 3;
    return top_next (GO_TO_UNLOAD_AREA, move_fsm_succeed);
}

/*
 * GO_TO_UNLOAD_AREA =move_fsm_failed=>
 *  => GO_TO_UNLOAD_AREA
 *   compute a new unload area.
 *   ask move FSM to go to the position using backward only.
 */
fsm_branch_t
top__GO_TO_UNLOAD_AREA__move_fsm_failed (void)
{
    asserv_position_t position;
    /* Go to unload area. */
    top_get_next_position_to_unload_puck (&position);
    /* Go there. */
    move_start (position, ASSERV_BACKWARD);
    return top_next (GO_TO_UNLOAD_AREA, move_fsm_failed);
}

/*
 * FUCK_UNLOAD_AREA =bot_move_succeed=>
 *  => UNLOAD_PUCKS
 *   unload all the pucks from the elevator.
 */
fsm_branch_t
top__FUCK_UNLOAD_AREA__bot_move_succeed (void)
{
    /* Unload elevator. */
    elvt_order = 3;
    return top_next (FUCK_UNLOAD_AREA, bot_move_succeed);
}

/*
 * FUCK_UNLOAD_AREA =bot_move_failed=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   move backward from the unload area using linear move.
 */
fsm_branch_t
top__FUCK_UNLOAD_AREA__bot_move_failed (void)
{
    /* Move forward. */
    asserv_move_linearly (PG_BORDER_DISTANCE);
    return top_next (FUCK_UNLOAD_AREA, bot_move_failed);
}

/*
 * UNLOAD_PUCKS =elevator_order_done=>
 *  => GO_AWAY_FROM_UNLOAD_AREA
 *   linear move.
 */
fsm_branch_t
top__UNLOAD_PUCKS__elevator_order_done (void)
{
    /* Close elevator. */
    elvt_order = 3;
    /* Move forward. */
    asserv_move_linearly (PG_BORDER_DISTANCE);
    return top_next (UNLOAD_PUCKS, elevator_order_done);
}

/*
 * UNLOAD_PUCKS =state_timeout=>
 *  => ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD
 *   we have failed, ask the elevator to put itself in a state where bot movement
 *   will not damage something.
 */
fsm_branch_t
top__UNLOAD_PUCKS__state_timeout (void)
{
    /* Elevator close. */
    elvt_order = 3;
    return top_next (UNLOAD_PUCKS, state_timeout);
}

/*
 * ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD =elevator_order_done=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   move backward from the unload area using linear move.
 */
fsm_branch_t
top__ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD__elevator_order_done (void)
{
    /* Move forward. */
    asserv_move_linearly (PG_BORDER_DISTANCE);
    return top_next (ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD, elevator_order_done);
}

/*
 * ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD =state_timeout=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   do the same as elevator_ready.
 */
fsm_branch_t
top__ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD__state_timeout (void)
{
    /* Yerk. */
    top__ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD__elevator_order_done ();
    return top_next (ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD, state_timeout);
}

/*
 * GO_AWAY_TO_RETRY_UNLOAD =bot_move_succeed=>
 *  => GO_TO_UNLOAD_AREA
 *   compute a new unload area.
 *   ask move FSM to go there using backward move only.
 */
fsm_branch_t
top__GO_AWAY_TO_RETRY_UNLOAD__bot_move_succeed (void)
{
    asserv_position_t position;
    /* Go to unload area. */
    top_get_next_position_to_unload_puck (&position);
    /* Go there. */
    move_start (position, ASSERV_BACKWARD);
    return top_next (GO_AWAY_TO_RETRY_UNLOAD, bot_move_succeed);
}

/*
 * GO_AWAY_TO_RETRY_UNLOAD =bot_move_failed=>
 * niceness => TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD
 *   decrement niceness.
 *   move forward using linear move.
 * no_more_niceness => GO_TO_UNLOAD_AREA
 *   do the same as move succeed.
 */
fsm_branch_t
top__GO_AWAY_TO_RETRY_UNLOAD__bot_move_failed (void)
{
    /* TODO: niceness. */
    if (1)
      {
	/* Move backward. */
	asserv_move_linearly (-PG_BORDER_DISTANCE);
	return top_next_branch (GO_AWAY_TO_RETRY_UNLOAD, bot_move_failed, niceness);
      }
    else
      {
	return top_next_branch (GO_AWAY_TO_RETRY_UNLOAD, bot_move_failed, no_more_niceness);
      }
}

/*
 * TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD =bot_move_succeed=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   move backward from the unload area using linear move.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD__bot_move_succeed (void)
{
    /* Move forward. */
    asserv_move_linearly (PG_BORDER_DISTANCE);
    return top_next (TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD, bot_move_succeed);
}

/*
 * TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD =bot_move_failed=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   do the same as succeed.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD__bot_move_failed (void)
{
    /* Yerk. */
    top__TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD__bot_move_succeed ();
    return top_next (TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD, bot_move_failed);
}

/*
 * FUCK_THE_DISTRIBUTOR =bot_move_succeed=>
 *  => WAIT_FOR_PUCKS
 *   let's cylinder FSM works until finished.
 */
fsm_branch_t
top__FUCK_THE_DISTRIBUTOR__bot_move_succeed (void)
{
    /* TODO: tell it to cylinder. */
    return top_next (FUCK_THE_DISTRIBUTOR, bot_move_succeed);
}

/*
 * FUCK_THE_DISTRIBUTOR =bot_move_failed=>
 * no_puck_or_still_time => GET_PUCK_FROM_DISTRIBUTOR
 *   get a new distributor and go there.
 * some_pucks_and_no_more_time => GO_TO_UNLOAD_AREA
 *   compute an unload area.
 *   ask move FSM to go there.
 */
fsm_branch_t
top__FUCK_THE_DISTRIBUTOR__bot_move_failed (void)
{
    /* Close cylinder. */
    cylinder_close_order = 1;
    if (!top_puck_inside_bot || chrono_remaining_time () > TOP_TIME_LIMIT)
      {
	/* Get next position. */
	asserv_position_t position;
	/* Go to distributor. */
	top_get_next_position_to_get_distributor (&position, &front_position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (FUCK_THE_DISTRIBUTOR, bot_move_failed,
				no_puck_or_still_time);
      }
    else
      {
	asserv_position_t position;
	/* Go to unload area. */
	top_get_next_position_to_unload_puck (&position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (FUCK_THE_DISTRIBUTOR, bot_move_failed,
				some_pucks_and_no_more_time);
      }
}

/*
 * WAIT_FOR_PUCKS =bot_is_full_of_pucks=>
 *  => GO_AWAY_FROM_DISTRIBUTOR
 *   remember unload area.
 */
fsm_branch_t
top__WAIT_FOR_PUCKS__bot_is_full_of_pucks (void)
{
    remember_distributor = 0;
    /* Backward move. */
    asserv_move_linearly (-PG_BORDER_DISTANCE);
    return top_next (WAIT_FOR_PUCKS, bot_is_full_of_pucks);
}

/*
 * WAIT_FOR_PUCKS =empty_distributor=>
 * no_puck_or_still_time => GO_AWAY_FROM_DISTRIBUTOR
 *   remember distributor.
 * some_pucks_and_no_more_time => GO_AWAY_FROM_DISTRIBUTOR
 *   remember unload area.
 */
fsm_branch_t
top__WAIT_FOR_PUCKS__empty_distributor (void)
{
    if (!top_puck_inside_bot || chrono_remaining_time () > TOP_TIME_LIMIT)
      {
	remember_distributor = 1;
	/* Backward move. */
	asserv_move_linearly (-PG_BORDER_DISTANCE);
	return top_next_branch (WAIT_FOR_PUCKS, empty_distributor, no_puck_or_still_time);
      }
    else
      {
	remember_distributor = 0;
	/* Backward move. */
	asserv_move_linearly (-PG_BORDER_DISTANCE);
	return top_next_branch (WAIT_FOR_PUCKS, empty_distributor, some_pucks_and_no_more_time);
      }
}

/*
 * WAIT_FOR_PUCKS =state_timeout=>
 * no_puck_or_still_time => GO_AWAY_FROM_DISTRIBUTOR
 *   remember distributor.
 * some_pucks_and_no_more_time => GO_AWAY_FROM_DISTRIBUTOR
 *   remember unload area.
 */
fsm_branch_t
top__WAIT_FOR_PUCKS__state_timeout (void)
{
    if (!top_puck_inside_bot || chrono_remaining_time () > TOP_TIME_LIMIT)
      {
	remember_distributor = 1;
	/* Backward move. */
	asserv_move_linearly (-PG_BORDER_DISTANCE);
	return top_next_branch (WAIT_FOR_PUCKS, state_timeout, no_puck_or_still_time);
      }
    else
      {
	remember_distributor = 0;
	/* Backward move. */
	asserv_move_linearly (-PG_BORDER_DISTANCE);
	return top_next_branch (WAIT_FOR_PUCKS, state_timeout, some_pucks_and_no_more_time);
      }
}

/*
 * GO_AWAY_FROM_UNLOAD_AREA =bot_move_succeed=>
 * more_than_six_pucks_or_no_next_position => GET_PUCK_FROM_DISTRIBUTOR
 *   get the next distributor position and launch move FSM to go there.
 * next_position_for_pucks_on_ground_exists => GET_PUCK_FROM_THE_GROUND
 *   go to the next position using move FSM.
 */
fsm_branch_t
top__GO_AWAY_FROM_UNLOAD_AREA__bot_move_succeed (void)
{
    /* Get next position. */
    asserv_position_t position;
    if (!top_get_next_position_to_get_puck_on_the_ground (&position, 0)
	|| top_total_puck_taken >= 6)
      {
	/* Go to distributor. */
	top_get_next_position_to_get_distributor (&position, &front_position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_succeed,
				more_than_six_pucks_or_no_next_position);
      }
    else
      {
	/* Go there. */
	move_start (position, 0);
	return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_succeed,
				next_position_for_pucks_on_ground_exists);
      }
}

/*
 * GO_AWAY_FROM_UNLOAD_AREA =bot_move_failed=>
 * niceness => TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA
 *   decrement niceness.
 *   move forward using linear move.
 * no_more_niceness_and_more_than_six_pucks_or_no_next_position => GET_PUCK_FROM_DISTRIBUTOR
 *   go to the next position using move FSM.
 * no_more_niceness_and_next_position_for_pucks_on_ground_exists => GET_PUCK_FROM_THE_GROUND
 *   go to the next position using move FSM.
 */
fsm_branch_t
top__GO_AWAY_FROM_UNLOAD_AREA__bot_move_failed (void)
{
    /* TODO: niceness. */
    if (1)
      {
	/* Move backward. */
	asserv_move_linearly (-PG_BORDER_DISTANCE);
	return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_failed, niceness);
      }
    else
      {
	asserv_position_t position;
	if (!top_get_next_position_to_get_puck_on_the_ground (&position, 0)
	    || top_total_puck_taken >= 6)
	  {
	    /* Go to distributor. */
	    top_get_next_position_to_get_distributor (&position, &front_position);
	    /* Go there. */
	    move_start (position, ASSERV_BACKWARD);
	    return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_failed,
				    no_more_niceness_and_more_than_six_pucks_or_no_next_position);
	  }
	else
	  {
	    /* Go there. */
	    move_start (position, 0);
	    return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_failed,
				    no_more_niceness_and_next_position_for_pucks_on_ground_exists);
	  }
      }
}

/*
 * TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA =bot_move_succeed=>
 *  => GO_AWAY_FROM_UNLOAD_AREA
 *   move backward using linear move.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA__bot_move_succeed (void)
{
    /* Move forward. */
    asserv_move_linearly (PG_BORDER_DISTANCE);
    return top_next (TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA, bot_move_succeed);
}

/*
 * TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA =bot_move_failed=>
 *  => GO_AWAY_FROM_UNLOAD_AREA
 *   do the same as succeed.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA__bot_move_failed (void)
{
    /* Yerk. */
    top__TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA__bot_move_succeed ();
    return top_next (TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA, bot_move_failed);
}

/*
 * CLEAN_FRONT_OF_DISTRIBUTOR =move_fsm_succeed=>
 *  => FUCK_THE_DISTRIBUTOR
 *   we are in front of the distributor: fuck it.
 */
fsm_branch_t
top__CLEAN_FRONT_OF_DISTRIBUTOR__move_fsm_succeed (void)
{
    /* Open cylinder. */
    cylinder_close_order = 0;
    /* Fuck the distributor. */
    asserv_go_to_the_wall (0);
    return top_next (CLEAN_FRONT_OF_DISTRIBUTOR, move_fsm_succeed);
}

/*
 * CLEAN_FRONT_OF_DISTRIBUTOR =move_fsm_failed=>
 *  => GET_PUCK_FROM_DISTRIBUTOR
 *   get a new distributor and go there.
 */
fsm_branch_t
top__CLEAN_FRONT_OF_DISTRIBUTOR__move_fsm_failed (void)
{
    /* Get next position. */
    asserv_position_t position;
    /* Go to distributor. */
    top_get_next_position_to_get_distributor (&position, &front_position);
    /* Go there. */
    move_start (position, ASSERV_BACKWARD);
    return top_next (CLEAN_FRONT_OF_DISTRIBUTOR, move_fsm_failed);
}

/*
 * GO_AWAY_FROM_DISTRIBUTOR =bot_move_succeed=>
 * go_to_distributor => GET_PUCK_FROM_DISTRIBUTOR
 *   get a new distributor position and go there.
 * go_to_unload => GO_TO_UNLOAD_AREA
 *   compute an unload area.
 *   ask the move FSM to go there.
 */
fsm_branch_t
top__GO_AWAY_FROM_DISTRIBUTOR__bot_move_succeed (void)
{
    if (remember_distributor)
      {
	/* Get next position. */
	asserv_position_t position;
	/* Go to distributor. */
	top_get_next_position_to_get_distributor (&position, &front_position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (GO_AWAY_FROM_DISTRIBUTOR, bot_move_succeed, go_to_distributor);
      }
    else
      {

	asserv_position_t position;
	/* Go to unload area. */
	top_get_next_position_to_unload_puck (&position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (GO_AWAY_FROM_DISTRIBUTOR, bot_move_succeed, go_to_unload);
      }
}

/*
 * GO_AWAY_FROM_DISTRIBUTOR =bot_move_failed=>
 * niceness => TRY_AGAIN_TO_GO_AWAY_FROM_DISTRIBUTOR
 *   decrement niceness.
 *   move forward using linear move.
 * no_more_niceness_and_go_to_distributor => GET_PUCK_FROM_DISTRIBUTOR
 *   get a new distributor position and go there.
 */
fsm_branch_t
top__GO_AWAY_FROM_DISTRIBUTOR__bot_move_failed (void)
{
    /* TODO: niceness. */
    if (1)
      {
	/* Linear move. */
	asserv_move_linearly (PG_BORDER_DISTANCE);
	return top_next_branch (GO_AWAY_FROM_DISTRIBUTOR, bot_move_failed, niceness);
      }
    else
      {
	/* Get next position. */
	asserv_position_t position;
	/* Go to distributor. */
	top_get_next_position_to_get_distributor (&position, &front_position);
	/* Go there. */
	move_start (position, ASSERV_BACKWARD);
	return top_next_branch (GO_AWAY_FROM_DISTRIBUTOR, bot_move_failed, no_more_niceness_and_go_to_distributor);
      }
}

/*
 * TRY_AGAIN_TO_GO_AWAY_FROM_DISTRIBUTOR =bot_move_succeed=>
 *  => GO_AWAY_FROM_DISTRIBUTOR
 *   move backward using linear move.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_FROM_DISTRIBUTOR__bot_move_succeed (void)
{
    /* Backward move. */
    asserv_move_linearly (-PG_BORDER_DISTANCE);
    return top_next (TRY_AGAIN_TO_GO_AWAY_FROM_DISTRIBUTOR, bot_move_succeed);
}

/*
 * TRY_AGAIN_TO_GO_AWAY_FROM_DISTRIBUTOR =bot_move_failed=>
 *  => GO_AWAY_FROM_DISTRIBUTOR
 *   do the same as succeed.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_FROM_DISTRIBUTOR__bot_move_failed (void)
{
    /* Backward move. */
    asserv_move_linearly (-PG_BORDER_DISTANCE);
    return top_next (TRY_AGAIN_TO_GO_AWAY_FROM_DISTRIBUTOR, bot_move_failed);
}

/*
 * PREPARE_ELEVATOR =elevator_order_done=>
 *  => FUCK_UNLOAD_AREA
 *   make the bot do a fuck the wall in backward until blocked.
 */
fsm_branch_t
top__PREPARE_ELEVATOR__elevator_order_done (void)
{
    /* Find the unload area. */
    asserv_go_to_the_wall (1);
    return top_next (PREPARE_ELEVATOR, elevator_order_done);
}

/*
 * PREPARE_ELEVATOR =state_timeout=>
 *  => GO_TO_UNLOAD_AREA
 *   compute a new unload area and retry.
 */
fsm_branch_t
top__PREPARE_ELEVATOR__state_timeout (void)
{
    /* Get next position. */
    asserv_position_t position;
    /* Go to unload area. */
    top_get_next_position_to_unload_puck (&position);
    /* Go there. */
    move_start (position, ASSERV_BACKWARD);
    return top_next (PREPARE_ELEVATOR, state_timeout);
}
