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

#include "io.h"

/**
 * The approach angle to face the distributor.
 */
extern int16_t approach_angle_;

/**
 * The samples bit field to collect.
 */
extern uint8_t sample_bitfield_;

/**
 * Configure the classifier (using the trap and the internal bit field) for
 * the first bit set to 1.
 * After the configuring the classifier, the bit will be reset to 0 to use the
 * next one when calling this function again.
 */
void
getsamples_configure_classifier (void);

/* Configure the classifier (using the trap and the internal bit field) for the first bit set to 1. */
void
getsamples_configure_classifier (void)
{
    uint8_t trap_num;
    /* Go through all the bits of the sample bit field */
    for (trap_num = 0; trap_num < trap_count; trap_num++)
      {
	/* Is the bit set? */
	if (bit_is_set (sample_bitfield_, trap_num))
	  {
	    /* Configure the classifier */
	    trap_setup_path_to_box (trap_num);
	    /* Reset this bit */
	    sample_bitfield_ &= ~_BV (trap_num);
	    /* Stop here */
	    return;
	  }
      }
}

/*
 * FACE_DISTRIBUTOR =bot_move_succeed=>
 *  => OPEN_INPUT_HOLE
 *   move the arm to open the input hole
 */
fsm_branch_t
getsamples__FACE_DISTRIBUTOR__bot_move_succeed (void)
{
    /* Move the arm to open the input hole to be able to take some samples */
    asserv_move_arm (-BOT_ARM_MIN_TO_OPEN, BOT_ARM_SPEED);
    return getsamples_next (FACE_DISTRIBUTOR, bot_move_succeed);
}

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
    fsm_handle_event (&top_fsm, TOP_EVENT_get_samples_fsm_finished);
    return getsamples_next (CLOSE_INPUT_HOLE, arm_move_succeed);
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
    if (sample_bitfield_)
      {
	/* Compute notifier */
	uint16_t arm_current_position = asserv_get_arm_position ();
	uint16_t arm_notify_position =
	    arm_current_position + BOT_ARM_NOTED_POSITION -
	    (arm_current_position % BOT_ARM_THIRD_ROUND);
	asserv_arm_set_position_reached (arm_notify_position);
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
 *  => FACE_DISTRIBUTOR
 *   do a goto angle to make the bot facing the distributor
 */
fsm_branch_t
getsamples__IDLE__start (void)
{
    /* Face the distributor */
    asserv_goto_angle (approach_angle_);
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
