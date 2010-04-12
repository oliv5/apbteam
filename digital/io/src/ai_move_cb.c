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
#include "trace_event.h"

#include "main.h"      /* main_post_event_for_top_fsm */
#include "modules/math/fixed/fixed.h"	/* fixed_* */
#include "modules/path/path.h"
#include "modules/utils/utils.h"
#include "modules/trace/trace.h"

#include "debug.host.h"

/**
 * The real radius of the obstacle.
 */
#define MOVE_REAL_OBSTACLE_RADIUS 150

/**
 * The radius of the obstacle for the path module.
 * It corresponds to the real radius of the obstacle plus the distance you
 * want to add to avoid it.
 */
#define MOVE_OBSTACLE_RADIUS (MOVE_REAL_OBSTACLE_RADIUS + 250)

/**
 * The generic validity time (in term of number of cycles).
 */
#define MOVE_OBSTACLE_VALIDITY (6 * 225)

/**
 * Verify after the computation of the obstacle, this shall only be called
 * after the function move_compute_obstacle_position.
 * \return  true if the position computed is in the table, false otherwise.
 */
uint8_t
move_obstacle_in_table (vect_t pos)
{
    if ((pos.x <= PG_WIDTH)
	&& (pos.x > 0)
	&& (pos.y <= PG_LENGTH)
	&& (pos.y > 0))
	return 0x1;
    else
	return 0x0;
}

/**
 * Easier function to get the next intermediate position from the path module.
 * @return
 *   - 0 if no path could be found ;
 *   - 1 if a path has been found.
 */
uint8_t
move_get_next_position (void)
{
    vect_t dst;
    /* Get the current position */
    position_t current_pos;
    asserv_get_position (&current_pos);
    /* Give the current position of the bot to the path module */
    path_endpoints (current_pos.v.x, current_pos.v.y,
		    move_data.final.v.x, move_data.final.v.y);
    /* Update the path module */
    path_update ();

    /* If the path is found. */
    if (path_get_next (&dst.x, &dst.y) != 0)
      {
	/* If it is not the last position. */
	if (dst.x != move_data.final.v.x || dst.y != move_data.final.v.y)
	  {
	    /* Not final position. */
	    move_data.final_move = 0;
	    /* Goto without angle. */
	    asserv_goto (dst.x, dst.y, move_data.backward_movement_allowed);
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

/**
 * Compute the obstacle position assuming it is right in front of us.
 * @param cur current position
 * @param obstacle the obstacle position computed
 */
void
move_compute_obstacle_position (position_t cur, vect_t *obstacle)
{
    int16_t dist;

    /* Convert the angle */
    uint32_t angle = cur.a;
    angle = angle << 8;

    /* Dirty fix: distance of the obstacle. */
    dist = (BOT_SIZE_FRONT + BOT_SIZE_BACK) / 2 + 350;
    /* Invert if last movement was backward. */
    if (asserv_get_last_moving_direction () == 2)
      {
	dist = -dist;
      }

    /* X */
    obstacle->x = cur.v.x + fixed_mul_f824 (fixed_cos_f824 (angle), dist);
    /* Y */
    obstacle->y = cur.v.y + fixed_mul_f824 (fixed_sin_f824 (angle), dist);
}

/**
 * Unique function to compute the obstacle position from here.
 */
void
move_obstacle_here (void)
{
    vect_t obstacle;
    /* Get the current position of the bot */
    position_t current;
    asserv_get_position (&current);
    /* Compute the obstacle position */
    move_compute_obstacle_position (current, &obstacle);
    /* Give it to the path module */
    /* only id the obstacle is in the table */
    if (move_obstacle_in_table (obstacle))
      {
	path_obstacle (0, obstacle.x, obstacle.y,
		       MOVE_OBSTACLE_RADIUS, 0, MOVE_OBSTACLE_VALIDITY);
	DPRINTF ("Obstacle pos x: %d, pos y: %d\n", obstacle.x, obstacle.y);
	TRACE (TRACE_MOVE__OBSTACLE, obstacle.x, obstacle.y);
      }
    else
      {
	DPRINTF ("Obstacle Ignored pos x: %d, pos y: %d\n", obstacle.x,
		 obstacle.y);
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
    /* Compute the obstacle position. */
    move_obstacle_here ();
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


