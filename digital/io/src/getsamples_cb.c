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
#include "main.h"	/* main_post_event_for_top_fsm */
#include "chrono.h"

#include "io.h"

/**
 * The distance to go backward from the distributor.
 */
#define GET_SAMPLES_MOVE_BACKWARD_DISTANCE 7

/**
 * Arm time out.
 */
#define GET_SAMPLES_ARM_TIMEOUT (5 * 225)

/**
 * 'Private' get samples data used internaly by the FSM.
 */
extern struct getsamples_data_t getsamples_data_;

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
    for (trap_num = out_left_box; trap_num < trap_count; trap_num++)
      {
	/* Is the bit set? */
	if (bit_is_set (getsamples_data_.sample_bitfield, trap_num))
	  {
	    /* Configure the classifier */
	    trap_setup_path_to_box (trap_num);
	    /* Reset this bit */
	    getsamples_data_.sample_bitfield &= ~_BV (trap_num);
	    /* Stop here */
	    return;
	  }
      }
}

/* Count the number of samples to take. */
uint8_t
getsamples_count_samples (void)
{
    uint8_t i, bit, count = 0;
    for (i = 0, bit = 1; i < 8; i++, bit <<= 1)
	if (getsamples_data_.sample_bitfield & bit)
	    count++;
    return count;
}
/*
 * MOVE_BACKWARD_FROM_DISTRIBUTOR =bot_move_succeed=>
 *  => TAKE_SAMPLES
 *   start taking some samples
 *   setup a timeout
 */
fsm_branch_t
getsamples__MOVE_BACKWARD_FROM_DISTRIBUTOR__bot_move_succeed (void)
{
    /* start taking some samples */
    getsamples_data_.arm_noted_position = asserv_get_arm_position () +
	BOT_ARM_NOTED_POSITION;
    asserv_arm_set_position_reached (getsamples_data_.arm_noted_position);
    asserv_move_arm (getsamples_count_samples () * BOT_ARM_THIRD_ROUND,
		     BOT_ARM_SPEED);
    /* Post an event for the top FSM to be waked up later */
    main_getsamples_wait_cycle = GET_SAMPLES_ARM_TIMEOUT;
    return getsamples_next (MOVE_BACKWARD_FROM_DISTRIBUTOR, bot_move_succeed);
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
 * CLOSE_INPUT_HOLE =wait_finished=>
 *  => IDLE
 *   timed out, give up
 *   tell the top FSM we have finished
 */
fsm_branch_t
getsamples__CLOSE_INPUT_HOLE__wait_finished (void)
{
    /* Give up, tell the top FSM we have finished */
    main_post_event_for_top_fsm = TOP_EVENT_get_samples_fsm_finished;
    return getsamples_next (CLOSE_INPUT_HOLE, wait_finished);
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
    main_post_event_for_top_fsm = TOP_EVENT_get_samples_fsm_finished;
    return getsamples_next (CLOSE_INPUT_HOLE, arm_move_succeed);
}

/*
 * TAKE_SAMPLES =wait_finished=>
 *  => MOVE_AWAY_FROM_DISTRIBUTOR
 *   timed out, give up
 *   go backward
 */
fsm_branch_t
getsamples__TAKE_SAMPLES__wait_finished (void)
{
    /* Go backward */
    asserv_move_linearly (-PG_DISTANCE_DISTRIBUTOR);
    return getsamples_next (TAKE_SAMPLES, wait_finished);
}

/*
 * TAKE_SAMPLES =arm_pass_noted_position=>
 * no_more => MOVE_AWAY_FROM_DISTRIBUTOR
 *   go backward
 * more => TAKE_SAMPLES
 *   prepare the classification of the taken sample
 *   take a new one
 *   reset the timeout
 */
fsm_branch_t
getsamples__TAKE_SAMPLES__arm_pass_noted_position (void)
{
    /* Prepare classification */
    getsamples_configure_classifier ();
    /* More samples? */
    if (getsamples_data_.sample_bitfield && !chrono_near_end_match ())
      {
	/* Compute notifier */
	getsamples_data_.arm_noted_position += BOT_ARM_THIRD_ROUND;
	asserv_arm_set_position_reached (getsamples_data_.arm_noted_position);
	/* Post an event for the top FSM to be waked up later */
	main_getsamples_wait_cycle = GET_SAMPLES_ARM_TIMEOUT;
	/* Continue to take sample */
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
    asserv_goto_angle (getsamples_data_.approach_angle);
    return getsamples_next (IDLE, start);
}

/*
 * MOVE_AWAY_FROM_DISTRIBUTOR =bot_move_succeed=>
 *  => CLOSE_INPUT_HOLE
 *   close input hole
 *   setup a close timeout
 */
fsm_branch_t
getsamples__MOVE_AWAY_FROM_DISTRIBUTOR__bot_move_succeed (void)
{
    /* Move the arm to close the input hole */
    asserv_move_arm (BOT_ARM_MIN_TO_OPEN + BOT_ARM_THIRD_ROUND, BOT_ARM_SPEED);
    /* Post an event for the top FSM to be waked up later */
    main_getsamples_wait_cycle = GET_SAMPLES_ARM_TIMEOUT;
    return getsamples_next (MOVE_AWAY_FROM_DISTRIBUTOR, bot_move_succeed);
}

/*
 * APPROACH_DISTRIBUTOR =bot_move_succeed=>
 *  => MOVE_BACKWARD_FROM_DISTRIBUTOR
 *   move a little bit backward from the distributor
 */
fsm_branch_t
getsamples__APPROACH_DISTRIBUTOR__bot_move_succeed (void)
{
    /* Move a little bit backward from the distributor */
    asserv_move_linearly (-GET_SAMPLES_MOVE_BACKWARD_DISTANCE);
    return getsamples_next (APPROACH_DISTRIBUTOR, bot_move_succeed);
}
