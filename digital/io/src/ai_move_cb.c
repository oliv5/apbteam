/* move_cb.c */
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
 * }}}
 * move:
 * move with avoid obstacle.
 */
#include "common.h"
#include "fsm.h"
#include "ai_cb.h"

#include "asserv.h"
#include "playground.h"
#include "move.h"
#include "bot.h"
#include "radar.h"
#include "events.h"

#include "main.h"      /* main_post_event_for_top_fsm */
#include "modules/math/fixed/fixed.h"	/* fixed_* */
#include "modules/path/path.h"
#include "modules/utils/utils.h"
#include "modules/trace/trace.h"

#include "debug.host.h"

#include <math.h>

/** Go or rotate toward position, returns 1 for linear move, 2 for angular
 * move. */
static uint8_t
move_go_or_rotate (vect_t dst, uint16_t angle, uint8_t with_angle,
		   uint8_t backward)
{
    position_t robot_position;
    /* Remember step. */
    move_data.step = dst;
    move_data.step_angle = angle;
    move_data.step_with_angle = with_angle;
    move_data.step_backward = backward;
    /* Compute angle to destination. */
    asserv_get_position (&robot_position);
    vect_t v = dst; vect_sub (&v, &robot_position.v);
    uint16_t dst_angle = atan2 (v.y, v.x) * ((1l << 16) / (2 * M_PI));
    if (backward & ASSERV_BACKWARD)
	dst_angle += 0x8000;
    if ((backward & ASSERV_REVERT_OK)
	&& (dst_angle ^ robot_position.a) & 0x8000)
	dst_angle += 0x8000;
    int16_t diff = dst_angle - robot_position.a;
    /* Move or rotate. */
    if (UTILS_ABS (diff) < 0x1000)
      {
	if (with_angle)
	    asserv_goto_xya (dst.x, dst.y, angle, backward);
	else
	    asserv_goto (dst.x, dst.y, backward);
	return 1;
      }
    else
      {
	asserv_goto_angle (dst_angle);
	return 2;
      }
}

/** Go to next position computed by path module, to be called by
 * move_path_init and move_path_next. Returns 1 for linear move, 2 for angular
 * move. */
static uint8_t
move_go_to_next (vect_t dst)
{
    uint8_t r;
    /* If it is not the last position. */
    if (dst.x != move_data.final.v.x || dst.y != move_data.final.v.y)
      {
	/* Not final position. */
	move_data.final_move = 0;
	/* Goto without angle. */
	r = move_go_or_rotate (dst, 0, 0, move_data.backward_movement_allowed
			       | (move_data.slow ? ASSERV_REVERT_OK : 0));
      }
    else
      {
	/* Final position. */
	move_data.final_move = 1;
	/* Goto with angle if requested. */
	r = move_go_or_rotate (dst, move_data.final.a, move_data.with_angle,
			       move_data.backward_movement_allowed);
      }
    TRACE (TRACE_MOVE__GO_TO, dst.x, dst.y);
    /* Reset try counter. */
    move_data.try_again_counter = 3;
    /* Next time, do not use slow. */
    move_data.slow = 0;
    return r;
}

/** Update and go to first position, return non zero if a path is found, 1 for
 * linear move, 2 for angular move. */
static uint8_t
move_path_init (void)
{
    uint8_t found;
    vect_t dst;
    /* Get the current position */
    position_t current_pos;
    asserv_get_position (&current_pos);
    /* Give the current position of the bot to the path module */
    path_endpoints (current_pos.v, move_data.final.v);
    /* Update the path module */
    move_data.slow = 0;
    path_update ();
    found = path_get_next (&dst);
    /* If not found, try to escape. */
    if (!found)
      {
	move_data.slow = 1;
	path_escape (8);
	path_update ();
	found = path_get_next (&dst);
      }
    /* If found, go. */
    if (found)
      {
	return move_go_to_next (dst);
      }
    else
      {
	/* Error, not final move. */
	move_data.final_move = 0;
	return 0;
      }
}

/** Go to next position in path. Returns 1 for linear move, 2 for angular
 * move. */
static uint8_t
move_path_next (void)
{
    vect_t dst;
    path_get_next (&dst);
    return move_go_to_next (dst);
}

/*
 * MOVE_IDLE =move_start=>
 * path_found_rotate => MOVE_ROTATING
 *   rotate towards next position.
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found => MOVE_IDLE
 *   post failure event.
 */
fsm_branch_t
ai__MOVE_IDLE__move_start (void)
{
    uint8_t next = move_path_init ();
    if (next)
      {
	if (next == 2)
	    return ai_next_branch (MOVE_IDLE, move_start, path_found_rotate);
	else
	    return ai_next_branch (MOVE_IDLE, move_start, path_found);
      }
    else
      {
	main_post_event (AI_EVENT_move_fsm_failed);
	return ai_next_branch (MOVE_IDLE, move_start, no_path_found);
      }
}

/*
 * MOVE_ROTATING =bot_move_succeed=>
 *  => MOVE_MOVING
 *   move to next position.
 */
fsm_branch_t
ai__MOVE_ROTATING__bot_move_succeed (void)
{
    if (move_data.step_with_angle)
	asserv_goto_xya (move_data.step.x, move_data.step.y,
			 move_data.step_angle, move_data.step_backward);
    else
	asserv_goto (move_data.step.x, move_data.step.y,
		     move_data.step_backward);
    return ai_next (MOVE_ROTATING, bot_move_succeed);
}

/*
 * MOVE_MOVING =bot_move_succeed=>
 * done => MOVE_IDLE
 *   post success event.
 * path_found_rotate => MOVE_ROTATING
 *   rotate towards next position.
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found => MOVE_IDLE
 *   post failure event.
 */
fsm_branch_t
ai__MOVE_MOVING__bot_move_succeed (void)
{
    if (move_data.final_move)
      {
	main_post_event (AI_EVENT_move_fsm_succeed);
	return ai_next_branch (MOVE_MOVING, bot_move_succeed, done);
      }
    else
      {
	uint8_t next = move_path_next ();
	if (next == 2)
	    return ai_next_branch (MOVE_MOVING, bot_move_succeed, path_found_rotate);
	else
	    return ai_next_branch (MOVE_MOVING, bot_move_succeed, path_found);
      }
    //return ai_next_branch (MOVE_MOVING, bot_move_succeed, no_path_found);
}

/*
 * MOVE_MOVING =bot_move_failed=>
 *  => MOVE_MOVING_BACKWARD_TO_TURN_FREELY
 *   reset final_move.
 *   move backward to turn freely.
 */
fsm_branch_t
ai__MOVE_MOVING__bot_move_failed (void)
{
    move_data.final_move = 0;
    /* Assume there is an obstacle in front of the robot. */
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    vect_t obstacle_pos;
    int16_t dist = asserv_get_last_moving_direction () == 1
	? BOT_SIZE_FRONT + MOVE_REAL_OBSTACLE_RADIUS
	: -(BOT_SIZE_BACK + MOVE_REAL_OBSTACLE_RADIUS);
    vect_from_polar_uf016 (&obstacle_pos, dist, robot_pos.a);
    vect_translate (&obstacle_pos, &robot_pos.v);
    path_obstacle (0, obstacle_pos, MOVE_OBSTACLE_RADIUS, 0,
		   MOVE_OBSTACLE_VALIDITY);
    /* Move backward to turn freely. */
    asserv_move_linearly (asserv_get_last_moving_direction () == 1 ?
			  - 300 : 300);
    return ai_next (MOVE_MOVING, bot_move_failed);
}

/*
 * MOVE_MOVING =obstacle_in_front=>
 *  => MOVE_WAIT_FOR_CLEAR_PATH
 *   reset final_move.
 *   stop the bot.
 */
fsm_branch_t
ai__MOVE_MOVING__obstacle_in_front (void)
{
    move_data.final_move = 0;
    asserv_stop_motor ();
    return ai_next (MOVE_MOVING, obstacle_in_front);
}

/*
 * MOVE_MOVING_BACKWARD_TO_TURN_FREELY =bot_move_succeed=>
 * path_found_rotate => MOVE_ROTATING
 *   rotate towards next position.
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found => MOVE_IDLE
 *   post failure event.
 */
fsm_branch_t
ai__MOVE_MOVING_BACKWARD_TO_TURN_FREELY__bot_move_succeed (void)
{
    uint8_t next = move_path_init ();
    if (next)
      {
	if (next == 2)
	    return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, path_found_rotate);
	else
	    return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, path_found);
      }
    else
      {
	main_post_event (AI_EVENT_move_fsm_failed);
	return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, no_path_found);
      }
}

/*
 * MOVE_MOVING_BACKWARD_TO_TURN_FREELY =bot_move_failed=>
 * path_found_rotate => MOVE_ROTATING
 *   rotate towards next position.
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found => MOVE_WAIT_FOR_CLEAR_PATH
 *   nothing to do.
 */
fsm_branch_t
ai__MOVE_MOVING_BACKWARD_TO_TURN_FREELY__bot_move_failed (void)
{
    uint8_t next = move_path_init ();
    if (next)
      {
	if (next == 2)
	    return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, path_found_rotate);
	else
	    return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, path_found);
      }
    else
	return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, no_path_found);
}

/*
 * MOVE_WAIT_FOR_CLEAR_PATH =state_timeout=>
 * path_found_rotate => MOVE_ROTATING
 *   rotate towards next position.
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found_and_try_again => MOVE_WAIT_FOR_CLEAR_PATH
 *   decrement counter.
 * no_path_found_and_no_try_again => MOVE_IDLE
 *   post failure.
 */
fsm_branch_t
ai__MOVE_WAIT_FOR_CLEAR_PATH__state_timeout (void)
{
    /* Try to move. */
    uint8_t next = move_path_init ();
    if (next)
      {
	if (next == 2)
	    return ai_next_branch (MOVE_WAIT_FOR_CLEAR_PATH, state_timeout, path_found_rotate);
	else
	    return ai_next_branch (MOVE_WAIT_FOR_CLEAR_PATH, state_timeout, path_found);
      }
    else
      {
	/* Error, no new position, should we try again? */
	if (--move_data.try_again_counter == 0)
	  {
	    main_post_event (AI_EVENT_move_fsm_failed);
	    return ai_next_branch (MOVE_WAIT_FOR_CLEAR_PATH, state_timeout,
				   no_path_found_and_no_try_again);
	  }
	else
	    return ai_next_branch (MOVE_WAIT_FOR_CLEAR_PATH, state_timeout,
				   no_path_found_and_try_again);
      }
}


