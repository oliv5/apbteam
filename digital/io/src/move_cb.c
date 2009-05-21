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
 * }}} */
#include "common.h"
#include "fsm.h"
#include "move_cb.h"

#include "asserv.h"
#include "playground.h"
#include "move.h"
#include "sharp.h"
#include "aquajim.h"
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
 * The sharp distance between the bot and the obstacle.
 */
#define MOVE_SHARP_DISTANCE 300

/**
 * The distance between the axis of the bot and the front sharp.
 */
#define MOVE_AXIS_FRONT_SHARP 150

/**
 * The standard distance of the obstacle.
 */
#define MOVE_OBSTACLE_DISTANCE \
    (MOVE_REAL_OBSTACLE_RADIUS + MOVE_SHARP_DISTANCE + MOVE_AXIS_FRONT_SHARP)

/**
 * The radius of the obstacle for the path module.
 * It corresponds to the real radius of the obstacle plus the distance you
 * want to add to avoid it.
 */
#define MOVE_OBSTACLE_RADIUS (MOVE_REAL_OBSTACLE_RADIUS + 250)

/**
 * The generic validity time (in term of number of cyles).
 */
#define MOVE_OBSTACLE_VALIDITY (6 * 225)

/**
 * Cycles count to ignore sharp event in the main loop.
 */
#define MOVE_MAIN_IGNORE_SHARP_EVENT (3 * 225)

/**
 * Number of cycles to wait before trying to read the sharps values again when
 * we are stopped.
 */
#define MOVE_WAIT_TIME_FOR_POOLING_SHARP (MOVE_MAIN_IGNORE_SHARP_EVENT)

/**
 * A detection offset for the sharps.
 */
#define MOVE_DETECTION_OFFSET 250

/**
 * Verify after the computation of the obstacle, this shall only be called
 * after the function move_compute_obstacle_position.
 * \return  true if the position computed is in the table, false otherwise.
 */
uint8_t
move_obstacle_in_table (move_position_t pos)
{
    if ((pos.x <= PG_WIDTH - MOVE_DETECTION_OFFSET) 
	&& (pos.x > MOVE_DETECTION_OFFSET)
	&& (pos.y <= PG_LENGTH - MOVE_DETECTION_OFFSET)
	&& (pos.y > MOVE_DETECTION_OFFSET))
	return 0x1;
    else
	return 0x0;
}

/**
 * Easier function to get the next intermediate position from the path module.
 * @param dst new destination position computed by the path module
 * @return
 *   - 0 if no path could be found ;
 *   - 1 if a path has been found.
 *   - 2 already at final place.
 */
uint8_t
move_get_next_position (move_position_t *dst)
{
    /* Are we at the final position. */
    if (move_data.final_move)
	return 2;
    /* Get the current position */
    asserv_position_t current_pos;
    asserv_get_position (&current_pos);
    /* Give the current position of the bot to the path module */
    path_endpoints (current_pos.x, current_pos.y,
		    move_data.final.x, move_data.final.y);
    TRACE (TRACE_MOVE__PATH_UPDATE);
    /* Update the path module */
    path_update ();

    /* If the path is found. */
    if (path_get_next (&dst->x, &dst->y) != 0)
      {
	/* If it is not the last position. */
	if (dst->x != move_data.final.x || dst->y != move_data.final.y)
	  {
	    /* Not final position. */
	    move_data.final_move = 0;
	    /* Goto without angle. */
	    asserv_goto (dst->x, dst->y,
			 move_data.backward_movement_allowed);
	  }
	else
	  {
	    /* Final position. */
	    move_data.final_move = 1;
	    /* Goto with angle. */
	    asserv_goto_xya (dst->x, dst->y, move_data.final.a,
			     move_data.backward_movement_allowed);
	  }
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
move_compute_obstacle_position (asserv_position_t cur,
				move_position_t *obstacle)
{
    int16_t dist;

    /* Convert the angle */
    uint32_t angle = cur.a;
    angle = angle << 8;

    /* Dirty fix: distance of the obstacle. */
    dist = BOT_LENGTH / 2 + 350;
    /* Invert if last movement was backward. */
    if (asserv_get_last_moving_direction () == 2)
      {
	dist = -dist;
      }

    /* X */
    obstacle->x = cur.x + fixed_mul_f824 (fixed_cos_f824 (angle),
					  dist);
    /* Y */
    obstacle->y = cur.y + fixed_mul_f824 (fixed_sin_f824 (angle),
					  dist);
}

/**
 * Unique function to compute the obstacle position from here.
 */
void
move_obstacle_here (void)
{
    /* Get the current position of the bot */
    asserv_position_t current;
    asserv_get_position (&current);
    /* Compute the obstacle position */
    move_compute_obstacle_position (current, &move_data.obstacle);
    /* Give it to the path module */
    /* only id the obstacle is in the table */
    if (move_obstacle_in_table (move_data.obstacle))
      {
	path_obstacle (0, move_data.obstacle.x, move_data.obstacle.y,
		       MOVE_OBSTACLE_RADIUS, MOVE_OBSTACLE_VALIDITY);
	DPRINTF ("Obstacle pos x : %d, pos y : %d\n", move_data.obstacle.x,
		 move_data.obstacle.y);
      }
    else
      {
	DPRINTF ("Obstacle Ignored pos x : %d, pos y : %d\n",
		 move_data.obstacle.x,
		 move_data.obstacle.y);
      }
}

/*
 * IDLE =start=>
 *  => MOVING
 *   ask the asserv to go to the computed position.
 */
fsm_branch_t
move__IDLE__start (void)
{
    /* ask the asserv to go to the computed position. */
    move_get_next_position (&move_data.intermediate);
    return move_next (IDLE, start);
}

/*
 * MOVING =bot_move_succeed=>
 * we_are_at_final_position => IDLE
 *   post an event for the top FSM to tell it we have finished.
 * position_intermediary => MOVING
 *   get next position computed by the path module.
 *   if next position is the final, use a goto_xya.
 *   otherwise go to the next intermediate position with goto.
 * no_intermediate_path_found => IDLE
 *   post an event for the top FSM to generate a failure.
 */
fsm_branch_t
move__MOVING__bot_move_succeed (void)
{
    uint8_t ret = move_get_next_position (&move_data.intermediate);
    if (ret == 2)
      {
	/* Post an event for the top FSM to tell it we have finished. */
	main_post_event_for_top_fsm = TOP_EVENT_move_fsm_succeed;
	return move_next_branch (MOVING, bot_move_succeed, we_are_at_final_position);
      }
    else if (ret == 1)
      {
	/* Nothing to do. */
	return move_next_branch (MOVING, bot_move_succeed, position_intermediary);
      }
    else
      {
	/* Post an event for the top FSM to generate a failure. */
	main_post_event_for_top_fsm = TOP_EVENT_move_fsm_failed;
	return move_next_branch (MOVING, bot_move_succeed, no_intermediate_path_found);
      }
}


/*
 * MOVING =bot_move_failed=>
 *  => MOVING_BACKWARD_TO_TURN_FREELY
 *   compute the obstacle position.
 *   move backward to turn freely.
 */
fsm_branch_t
move__MOVING__bot_move_failed (void)
{
    /* Compute the obstacle position. */
    move_obstacle_here ();
    /* Move backward to turn freely. */
    asserv_move_linearly (asserv_get_last_moving_direction () == 1 ?
			  - 300 : 300);
    return move_next (MOVING, bot_move_failed);
}

/*
 * MOVING =obstacle_in_front=>
 *  => WAIT_FOR_CLEAR_PATH
 *   stop the bot.
 */
fsm_branch_t
move__MOVING__obstacle_in_front (void)
{
    /* Stop the bot. */
    asserv_stop_motor ();
    return move_next (MOVING, obstacle_in_front);
}

/*
 * MOVING_BACKWARD_TO_TURN_FREELY =bot_move_succeed=>
 * intermediate_path_found => MOVING
 *   get next intermediate position from path module.
 * no_intermediate_path_found => IDLE
 *   post an event for the top FSM to generate a failure.
 */
fsm_branch_t
move__MOVING_BACKWARD_TO_TURN_FREELY__bot_move_succeed (void)
{
    uint8_t ret = move_get_next_position (&move_data.intermediate);
    if (ret == 1)
      {
	/* Nothing to do. */
	return move_next_branch (MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, intermediate_path_found);
      }
    else
      {
	/* Post an event for the top FSM to generate a failure. */
	main_post_event_for_top_fsm = TOP_EVENT_move_fsm_failed;
	return move_next_branch (MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, no_intermediate_path_found);
      }
}

/*
 * MOVING_BACKWARD_TO_TURN_FREELY =bot_move_failed=>
 * intermediate_path_found => MOVING
 *   get next intermediate position from path module
 * no_intermediate_path_found => WAIT_FOR_CLEAR_PATH
 *   nothing to do.
 */
fsm_branch_t
move__MOVING_BACKWARD_TO_TURN_FREELY__bot_move_failed (void)
{
    uint8_t ret = move_get_next_position (&move_data.intermediate);
    if (ret == 1)
      {
	/* Nothing to do. */
	return move_next_branch (MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, intermediate_path_found);
      }
    else
      {
	/* Nothing to do. */
	return move_next_branch (MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, no_intermediate_path_found);
      }
}

/*
 * WAIT_FOR_CLEAR_PATH =state_timeout=>
 * no_more_obstacle_or_next_position => MOVING
 *   get next position computed by the path module.
 *   if next position is the final, use a goto_xya.
 *   otherwise go to the next intermediate position with goto.
 * obstacle_and_no_intermediate_path_found_and_try_again => WAIT_FOR_CLEAR_PATH
 *   decrement counter.
 * obstacle_and_no_intermediate_path_found_and_no_try_again => IDLE
 *   post an event for the top FSM to generate a failure.
 */
fsm_branch_t
move__WAIT_FOR_CLEAR_PATH__state_timeout (void)
{
    if (sharp_path_obstrued (asserv_get_last_moving_direction ()))
	move_obstacle_here ();
    uint8_t ret = move_get_next_position (&move_data.intermediate);
    if (ret == 1)
      {
	/* Go to position. */
	return move_next_branch (WAIT_FOR_CLEAR_PATH, state_timeout,
				 no_more_obstacle_or_next_position);
      }
    else
      {
	/* Error, no new position, should we try again? */
	if (--move_data.try_again_counter == 0)
	  {
	    /* Post an event for the top FSM to generate a failure. */
	    main_post_event_for_top_fsm = TOP_EVENT_move_fsm_failed;
	    return move_next_branch (WAIT_FOR_CLEAR_PATH, state_timeout,
				     obstacle_and_no_intermediate_path_found_and_no_try_again);
	  }
	else
	    return move_next_branch (WAIT_FOR_CLEAR_PATH, state_timeout,
				     obstacle_and_no_intermediate_path_found_and_try_again);
      }
}


