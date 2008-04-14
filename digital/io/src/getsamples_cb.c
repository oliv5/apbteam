/* getsamples_cb.c - getsamples FSM callbacks. */
/*  {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include "fsm.h"
#include "getsamples_cb.h"
#include "getsamples.h"
#include "asserv.h"
#include "trap.h"
#include "move.h"

#include "giboulee.h"	/* BOT_ */
#include "playground.h"	/* PG_* */

/*
 * OPEN_INPUT_HOLE =arm_move_succeed=>
 *  => APPROACH_DISTRIBUTOR
 *   start approaching the distributor now
 */
fsm_branch_t
getsamples__OPEN_INPUT_HOLE__arm_move_succeed (void)
{
    /* Approach the distributor */
    asserv_go_to_distributor ();
    return getsamples_next (OPEN_INPUT_HOLE, arm_move_succeed);
}

/*
 * CLOSE_INPUT_HOLE =arm_move_succeed=>
 *  => IDLE
 *   tell the top FSM we have finished
 */
fsm_branch_t
getsamples__CLOSE_INPUT_HOLE__arm_move_succeed (void)
{
    /* Tell the top FSM we have finished */
    fsm_handle_event (&top_fsm, getsamples_data.event);
    return getsamples_next (CLOSE_INPUT_HOLE, arm_move_succeed);
}

/*
 * GO_IN_FRONT_OF_DISTRIBUTOR =bot_move_succeed=>
 *  => OPEN_INPUT_HOLE
 *   move the arm to open the input hole
 */
fsm_branch_t
getsamples__GO_IN_FRONT_OF_DISTRIBUTOR__bot_move_succeed (void)
{
    /* Move the arm to open the input hole to be able to take some samples */
    asserv_close_input_hole ();
    return getsamples_next (GO_IN_FRONT_OF_DISTRIBUTOR, bot_move_succeed);
}

/*
 * TAKE_SAMPLES =arm_pass_noted_position=>
 * no_more => MOVE_AWAY_FROM_DISTRIBUTOR
 *   go backward
 * more => TAKE_SAMPLES
 *   prepare the classification of the taken sample
 *   take a new one
 */
fsm_branch_t
getsamples__TAKE_SAMPLES__arm_pass_noted_position (void)
{
    /* More samples? */
    if (getsamples_data.sample_bitfield)
      {
	/* Prepare classification */
	getsamples_configure_classifier ();
	/* Continue to take sample */
	asserv_move_arm (BOT_ARM_THIRD_ROUND, BOT_ARM_SPEED);
	return getsamples_next_branch (TAKE_SAMPLES, arm_pass_noted_position, more);
      }
    else
      {
	/* Go backward */
	asserv_move_linearly (-PG_DISTANCE_DISTRIBUTOR);
	return getsamples_next_branch (TAKE_SAMPLES, arm_pass_noted_position, no_more);
      }
}

/*
 * IDLE =start=>
 *  => GO_IN_FRONT_OF_DISTRIBUTOR
 *   start going in front of the desired distributor
 */
fsm_branch_t
getsamples__IDLE__start (void)
{
    /* Move to the desired distributor */
    move_start (getsamples_data.distributor_x,
		getsamples_data.distributor_y);
    return getsamples_next (IDLE, start);
}

/*
 * MOVE_AWAY_FROM_DISTRIBUTOR =bot_move_succeed=>
 *  => CLOSE_INPUT_HOLE
 *   close input hole
 */
fsm_branch_t
getsamples__MOVE_AWAY_FROM_DISTRIBUTOR__bot_move_succeed (void)
{
    /* Move the arm to close the input hole */
    asserv_close_input_hole ();
    return getsamples_next (MOVE_AWAY_FROM_DISTRIBUTOR, bot_move_succeed);
}

/*
 * APPROACH_DISTRIBUTOR =bot_move_succeed=>
 *  => TAKE_SAMPLES
 *   start taking some samples
 */
fsm_branch_t
getsamples__APPROACH_DISTRIBUTOR__bot_move_succeed (void)
{
    /* start taking some samples */
    asserv_move_arm (BOT_ARM_THIRD_ROUND, BOT_ARM_SPEED);
    return getsamples_next (APPROACH_DISTRIBUTOR, bot_move_succeed);
}
