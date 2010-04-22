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
#include "trace_event.h"

#include "main.h"      /* main_post_event_for_top_fsm */
#include "modules/math/fixed/fixed.h"	/* fixed_* */
#include "modules/path/path.h"
#include "modules/utils/utils.h"
#include "modules/trace/trace.h"

#include "debug.host.h"

/**
 * Easier function to get the next intermediate position from the path module.
 * @return
 *   - 0 if no path could be found ;
 *   - 1 if a path has been found.
 */
uint8_t
move_get_next_position (void)
{
    uint8_t found;
    uint8_t slow = 0;
    vect_t dst;
    /* Get the current position */
    position_t current_pos;
    asserv_get_position (&current_pos);
    /* Give the current position of the bot to the path module */
    path_endpoints (current_pos.v.x, current_pos.v.y,
		    move_data.final.v.x, move_data.final.v.y);
    /* Update the path module */
    path_update ();
    found = path_get_next (&dst.x, &dst.y);
    /* If not found, try to escape. */
    if (!found)
      {
	slow = 1;
	path_escape (8);
	path_update ();
	found = path_get_next (&dst.x, &dst.y);
      }
    /* If the path is found, move. */
    if (found)
      {
	/* If it is not the last position. */
	if (dst.x != move_data.final.v.x || dst.y != move_data.final.v.y)
	  {
	    /* Not final position. */
	    move_data.final_move = 0;
	    /* Goto without angle. */
	    asserv_goto (dst.x, dst.y, move_data.backward_movement_allowed
			 | (slow ? ASSERV_REVERT_OK : 0));
	  }
	else
	  {
	    /* Final position. */
	    move_data.final_move = 1;
	    /* Goto with angle. */
	    asserv_goto_xya (dst.x, dst.y, move_data.final.a,
			     move_data.backward_movement_allowed);
	  }
	move_data.step = dst;
	TRACE (TRACE_MOVE__GO_TO, (u16) current_pos.v.x,
	       (u16) current_pos.v.y, current_pos.a, dst.x, dst.y,
	       move_data.final.a);
	/* Reset try counter. */
	move_data.try_again_counter = 3;
	return 1;
      }
    else
      {
	/* Error, not final move. */
	move_data.final_move = 0;
	return 0;
      }
}

/*
 * MOVE_IDLE =move_start=>
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found => MOVE_IDLE
 *   post failure event.
 */
fsm_branch_t
ai__MOVE_IDLE__move_start (void)
{
    if (move_get_next_position ())
	return ai_next_branch (MOVE_IDLE, move_start, path_found);
    else
      {
	main_post_event_for_top_fsm = AI_EVENT_move_fsm_failed;
	return ai_next_branch (MOVE_IDLE, move_start, no_path_found);
      }
}

/*
 * MOVE_MOVING =bot_move_succeed=>
 * done => MOVE_IDLE
 *   post success event.
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
	main_post_event_for_top_fsm = AI_EVENT_move_fsm_succeed;
	return ai_next_branch (MOVE_MOVING, bot_move_succeed, done);
      }
    else if (move_get_next_position ())
      {
	return ai_next_branch (MOVE_MOVING, bot_move_succeed, path_found);
      }
    else
      {
	main_post_event_for_top_fsm = AI_EVENT_move_fsm_failed;
	return ai_next_branch (MOVE_MOVING, bot_move_succeed, no_path_found);
      }
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
    path_obstacle (0, obstacle_pos.x, obstacle_pos.y, MOVE_OBSTACLE_RADIUS, 0,
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
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found => MOVE_IDLE
 *   post failure event.
 */
fsm_branch_t
ai__MOVE_MOVING_BACKWARD_TO_TURN_FREELY__bot_move_succeed (void)
{
    if (move_get_next_position ())
      {
	return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, path_found);
      }
    else
      {
	main_post_event_for_top_fsm = AI_EVENT_move_fsm_failed;
	return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, no_path_found);
      }
}

/*
 * MOVE_MOVING_BACKWARD_TO_TURN_FREELY =bot_move_failed=>
 * path_found => MOVE_MOVING
 *   move to next position.
 * no_path_found => MOVE_WAIT_FOR_CLEAR_PATH
 *   nothing to do.
 */
fsm_branch_t
ai__MOVE_MOVING_BACKWARD_TO_TURN_FREELY__bot_move_failed (void)
{
    if (move_get_next_position ())
	return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, path_found);
    else
	return ai_next_branch (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, no_path_found);
}

/*
 * MOVE_WAIT_FOR_CLEAR_PATH =state_timeout=>
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
    if (move_get_next_position ())
      {
	return ai_next_branch (MOVE_WAIT_FOR_CLEAR_PATH, state_timeout,
			       path_found);
      }
    else
      {
	/* Error, no new position, should we try again? */
	if (--move_data.try_again_counter == 0)
	  {
	    main_post_event_for_top_fsm = AI_EVENT_move_fsm_failed;
	    return ai_next_branch (MOVE_WAIT_FOR_CLEAR_PATH, state_timeout,
				   no_path_found_and_no_try_again);
	  }
	else
	    return ai_next_branch (MOVE_WAIT_FOR_CLEAR_PATH, state_timeout,
				   no_path_found_and_try_again);
      }
}


