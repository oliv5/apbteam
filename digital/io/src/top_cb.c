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
 *   we have failed to do a move, go the distributor.
 */
fsm_branch_t
top__GET_PUCK_FROM_THE_GROUND__move_fsm_failed (void)
{
    return top_next (GET_PUCK_FROM_THE_GROUND, move_fsm_failed);
}

/*
 * GET_PUCK_FROM_THE_GROUND =bot_is_full_of_puck=>
 *  => STOP_TO_GO_TO_UNLOAD_AREA
 *   stop move FSM.
 */
fsm_branch_t
top__GET_PUCK_FROM_THE_GROUND__bot_is_full_of_puck (void)
{
    return top_next (GET_PUCK_FROM_THE_GROUND, bot_is_full_of_puck);
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


