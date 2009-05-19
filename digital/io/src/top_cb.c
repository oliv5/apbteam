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
#include "asserv.h"
#include "playground.h"
#include "move.h"	/* move FSM */

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
    return top_next_branch (GET_PUCK_FROM_THE_GROUND, move_fsm_succeed, already_six_pucks_or_no_next_position);
    return top_next_branch (GET_PUCK_FROM_THE_GROUND, move_fsm_succeed, next_position_exists);
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
    return top_next (GET_PUCK_FROM_THE_GROUND, bot_is_full_of_pucks);
}

/*
 * GET_PUCK_FROM_THE_GROUND =state_timeout=>
 *  => STOP_TO_GET_PUCK_FROM_DISTRIBUTOR
 *   too much time lost to get puck from the ground, stop move FSM.
 */
fsm_branch_t
top__GET_PUCK_FROM_THE_GROUND__state_timeout (void)
{
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
    return top_next_branch (GET_PUCK_FROM_DISTRIBUTOR, move_fsm_failed, no_puck_or_still_time);
    return top_next_branch (GET_PUCK_FROM_DISTRIBUTOR, move_fsm_failed, some_pucks_and_no_more_time);
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
    return top_next (STOP_TO_GET_PUCK_FROM_DISTRIBUTOR, move_fsm_stopped);
}

/*
 * GO_TO_UNLOAD_AREA =move_fsm_succeed=>
 *  => FUCK_UNLOAD_AREA
 *   make the bot do a fuck the wall in backward until blocked.
 */
fsm_branch_t
top__GO_TO_UNLOAD_AREA__move_fsm_succeed (void)
{
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
    return top_next (FUCK_UNLOAD_AREA, bot_move_failed);
}

/*
 * UNLOAD_PUCKS =elevator_is_closed=>
 *  => GO_AWAY_FROM_UNLOAD_AREA
 *   linear move.
 */
fsm_branch_t
top__UNLOAD_PUCKS__elevator_is_closed (void)
{
    return top_next (UNLOAD_PUCKS, elevator_is_closed);
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
    return top_next (UNLOAD_PUCKS, state_timeout);
}

/*
 * ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD =elevator_is_closed=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   move backward from the unload area using linear move.
 */
fsm_branch_t
top__ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD__elevator_is_closed (void)
{
    return top_next (ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD, elevator_is_closed);
}

/*
 * ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD =state_timeout=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   do the same as elevator_ready.
 */
fsm_branch_t
top__ELEVATOR_READY_TO_GO_AWAY_TO_RETRY_UNLOAD__state_timeout (void)
{
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
    return top_next_branch (GO_AWAY_TO_RETRY_UNLOAD, bot_move_failed, niceness);
    return top_next_branch (GO_AWAY_TO_RETRY_UNLOAD, bot_move_failed, no_more_niceness);
}

/*
 * TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD =bot_move_succeed=>
 *  => GO_AWAY_TO_RETRY_UNLOAD
 *   move backward from the unload area using linear move.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_TO_RETRY_UNLOAD__bot_move_succeed (void)
{
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
    return top_next_branch (FUCK_THE_DISTRIBUTOR, bot_move_failed, no_puck_or_still_time);
    return top_next_branch (FUCK_THE_DISTRIBUTOR, bot_move_failed, some_pucks_and_no_more_time);
}

/*
 * WAIT_FOR_PUCKS =bot_is_full_of_pucks=>
 *  => GO_TO_UNLOAD_AREA
 *   compute an unload area.
 *   ask the move FSM to go there.
 */
fsm_branch_t
top__WAIT_FOR_PUCKS__bot_is_full_of_pucks (void)
{
    return top_next (WAIT_FOR_PUCKS, bot_is_full_of_pucks);
}

/*
 * WAIT_FOR_PUCKS =empty_distributor=>
 * no_puck_or_still_time => GET_PUCK_FROM_DISTRIBUTOR
 *   get a new distributor position and go there.
 * some_pucks_and_no_more_time => GO_TO_UNLOAD_AREA
 *   compute an unload area and go there.
 */
fsm_branch_t
top__WAIT_FOR_PUCKS__empty_distributor (void)
{
    return top_next_branch (WAIT_FOR_PUCKS, empty_distributor, no_puck_or_still_time);
    return top_next_branch (WAIT_FOR_PUCKS, empty_distributor, some_pucks_and_no_more_time);
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
    return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_succeed, more_than_six_pucks_or_no_next_position);
    return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_succeed, next_position_for_pucks_on_ground_exists);
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
    return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_failed, niceness);
    return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_failed, no_more_niceness_and_more_than_six_pucks_or_no_next_position);
    return top_next_branch (GO_AWAY_FROM_UNLOAD_AREA, bot_move_failed, no_more_niceness_and_next_position_for_pucks_on_ground_exists);
}

/*
 * TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA =bot_move_succeed=>
 *  => GO_AWAY_FROM_UNLOAD_AREA
 *   move backward using linear move.
 */
fsm_branch_t
top__TRY_AGAIN_TO_GO_AWAY_FROM_UNLOAD_AREA__bot_move_succeed (void)
{
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
    return top_next (CLEAN_FRONT_OF_DISTRIBUTOR, move_fsm_failed);
}


