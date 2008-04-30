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

#include "path.h"
#include "asserv.h"
#include "playground.h"
#include "move.h"
#include "sharp.h"

#include "main.h"      /* main_post_event_for_top_fsm */
#include "modules/math/fixed/fixed.h"	/* fixed_* */

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
#define MOVE_OBSTACLE_VALIDITY (4 * 225)

/**
 * Cycles count to ignore sharp event in the main loop.
 */
#define MOVE_MAIN_IGNORE_SHARP_EVENT (3 * 225)

/**
 * Number of cycles to wait before trying to read the sharps values again when
 * we are stopped.
 */
#define MOVE_WAIT_TIME_FOR_POOLING_SHARP (50)

/**
 * Easier function to get the next intermediate positon from the path module.
 * @param dst new destination position computed by the path module
 * @return
 *   - 0 if no path could be found ;
 *   - 1 if a path has been found.
 */
uint8_t
move_get_next_position (move_position_t *dst)
{
    /* Get the current position */
    asserv_position_t current_pos;
    asserv_get_position (&current_pos);
    /* Give the current position of the bot to the path module */
    path_endpoints (current_pos.x, current_pos.y,
		    move_data.final.x, move_data.final.y);
    /* Update the path module */
    path_update ();

    /* Retrieve next path coordinate */
    if (!path_get_next (&dst->x, &dst->y))
      {
	DPRINTF ("Could not compute any path to avoid obstacle!\n");
	return 0;
      }
    main_sharp_ignore_event = MOVE_MAIN_IGNORE_SHARP_EVENT;
    DPRINTF ("Computed path is (%d ; %d)\n", dst->x, dst->y);
    return 1;
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
    /* Convert the angle */
    uint32_t angle = cur.a;
    /* Change angle when going backward */
    if (asserv_get_moving_direction () == 2)
	angle += 0x8000;
    angle = angle << 8;
    DPRINTF ("We are at (%d ; %d ; %x)\n", cur.x, cur.y, cur.a);
    /* X */
    obstacle->x = cur.x + fixed_mul_f824 (fixed_cos_f824 (angle),
					  MOVE_OBSTACLE_DISTANCE);
    /* Y */
    obstacle->y = cur.y + fixed_mul_f824 (fixed_sin_f824 (angle),
					  MOVE_OBSTACLE_DISTANCE);
    DPRINTF ("Computed obstacle (%d ; %d)\n", obstacle->x, obstacle->y);
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
    path_obstacle (0, move_data.obstacle.x, move_data.obstacle.y,
		   MOVE_OBSTACLE_RADIUS, MOVE_OBSTACLE_VALIDITY);
}

/**
 * Unique function after moving backward to have unique code.
 * @return the value of the move_get_next_position.
 */
uint8_t
move_after_moving_backward (void)
{
    /* Give the current position of the bot to the path module */
    if (move_get_next_position (&move_data.intermediate))
      {
	/* Go to the next position */
	if (move_data.backward_movement_allowed)
	    asserv_goto_back (move_data.intermediate.x, move_data.intermediate.y);
	else
	    asserv_goto (move_data.intermediate.x, move_data.intermediate.y);
	return 1;
      }
    else
	return 0;
}

/*
 * WAIT_FOR_CLEAR_PATH =wait_finished=>
 * no_obstacle => MOVING_TO_FINAL_POSITION
 *   check for obstacle using stored moving direction
 *   try to go the final position
 * obstacle => WAIT_FOR_CLEAR_PATH
 *   check for obstacle using stored moving direction
 *   post an event for the top FSM to be waked up later
 */
fsm_branch_t
move__WAIT_FOR_CLEAR_PATH__wait_finished (void)
{
    if (!sharp_path_obstrued (move_data.cached_moving_direction))
      {
	/* Try to go to the final position */
	if (move_data.backward_movement_allowed)
	    asserv_goto_back (move_data.final.x, move_data.final.y);
	else
	    asserv_goto (move_data.final.x, move_data.final.y);
	return move_next_branch (WAIT_FOR_CLEAR_PATH, wait_finished, no_obstacle);
      }
    else
      {
	/* Post an event for the top FSM to be waked up later */
	main_move_wait_cycle = MOVE_WAIT_TIME_FOR_POOLING_SHARP;
	return move_next_branch (WAIT_FOR_CLEAR_PATH, wait_finished, obstacle);
      }
}

/*
 * IDLE =start=>
 *  => MOVING_TO_FINAL_POSITION
 *   ask the asserv to go to the final position
 */
fsm_branch_t
move__IDLE__start (void)
{
    /* Go to the destination position */
    if (move_data.backward_movement_allowed)
	asserv_goto_back (move_data.final.x, move_data.final.y);
    else
	asserv_goto (move_data.final.x, move_data.final.y);
    return move_next (IDLE, start);
}

/*
 * MOVING_TO_INTERMEDIATE_POSITION =bot_move_succeed=>
 * final_position => IDLE
 *   post an event for the top FSM to tell we have finished
 * position_intermediary => MOVING_TO_INTERMEDIATE_POSITION
 *   go to the next intermediate position computed by the path module
 * no_intermediate_path_found => WAIT_FOR_CLEAR_PATH
 *   store current moving direction
 *   stop the bot
 *   post an event for the top FSM to be waked up later
 */
fsm_branch_t
move__MOVING_TO_INTERMEDIATE_POSITION__bot_move_succeed (void)
{
    if ((move_data.final.x == move_data.intermediate.x) &&
	(move_data.final.y == move_data.intermediate.y))
      {
	/* Post an event for the top FSM to tell we have finished */
	main_post_event_for_top_fsm = TOP_EVENT_move_fsm_finished;
	return move_next_branch (MOVING_TO_INTERMEDIATE_POSITION, bot_move_succeed, final_position);
      }
    else
      {
	/* Get next position */
	if (move_get_next_position (&move_data.intermediate))
	  {
	    /* Go to the next intermediate position */
	    if (move_data.backward_movement_allowed)
		asserv_goto_back (move_data.intermediate.x, move_data.intermediate.y);
	    else
		asserv_goto (move_data.intermediate.x, move_data.intermediate.y);
	    return move_next_branch (MOVING_TO_INTERMEDIATE_POSITION, bot_move_succeed, position_intermediary);
	  }
	else
	  {
	    /* Store current moving direction */
	    move_data.cached_moving_direction = asserv_get_moving_direction ();
	    /* Stop the bot */
	    asserv_stop_motor ();
	    /* Post an event for the top FSM to be waked up later */
	    main_move_wait_cycle = MOVE_WAIT_TIME_FOR_POOLING_SHARP;
	    return move_next_branch (MOVING_TO_INTERMEDIATE_POSITION, bot_move_succeed, no_intermediate_path_found);
	  }
      }
}

/*
 * MOVING_TO_INTERMEDIATE_POSITION =bot_move_obstacle=>
 * intermediate_path_found => MOVING_TO_INTERMEDIATE_POSITION
 *   compute the obstacle position
 *   get next intermediate position from path module
 *   go to next intermediate position
 * no_intermediate_path_found => WAIT_FOR_CLEAR_PATH
 *   compute the obstacle position
 *   get next intermediate position from path module failed
 *   store current moving direction
 *   stop the bot
 *   post an event for the top FSM to be waked up later
 */
fsm_branch_t
move__MOVING_TO_INTERMEDIATE_POSITION__bot_move_obstacle (void)
{
    /* Compute obstacle position */
    move_obstacle_here ();
    /* Get next position */
    if (move_get_next_position (&move_data.intermediate))
      {
	/* Go to the next intermediate position */
	if (move_data.backward_movement_allowed)
	    asserv_goto_back (move_data.intermediate.x, move_data.intermediate.y);
	else
	    asserv_goto (move_data.intermediate.x, move_data.intermediate.y);
	return move_next_branch (MOVING_TO_INTERMEDIATE_POSITION, bot_move_obstacle, intermediate_path_found);
      }
    else
      {
	/* Store current moving direction */
	move_data.cached_moving_direction = asserv_get_moving_direction ();
	/* Stop the bot */
	asserv_stop_motor ();
	/* Post an event for the top FSM to be waked up later */
	main_move_wait_cycle = MOVE_WAIT_TIME_FOR_POOLING_SHARP;
	return move_next_branch (MOVING_TO_INTERMEDIATE_POSITION, bot_move_obstacle, no_intermediate_path_found);
      }
}

/*
 * MOVING_TO_INTERMEDIATE_POSITION =bot_move_failed=>
 *  => MOVING_BACKWARD
 *   store the current position of the obstacle
 *   move backward to turn freely
 */
fsm_branch_t
move__MOVING_TO_INTERMEDIATE_POSITION__bot_move_failed (void)
{
    /* Compute obstacle position */
    move_obstacle_here ();
    /* Go backward */
    asserv_move_linearly (-PG_MOVE_DISTANCE); 
    return move_next (MOVING_TO_INTERMEDIATE_POSITION, bot_move_failed);
}

/*
 * MOVING_BACKWARD =bot_move_failed=>
 * intermediate_path_found => MOVING_TO_INTERMEDIATE_POSITION
 *   get next intermediate position from path module
 * no_intermediate_path_found => WAIT_FOR_CLEAR_PATH
 *   get next intermediate position from path module, failed
 *   stop the bot
 *   post an event for the top FSM to be waked up later
 */
fsm_branch_t
move__MOVING_BACKWARD__bot_move_failed (void)
{
    /* Call generic function */
    if (move_after_moving_backward ())
      {
	return move_next_branch (MOVING_BACKWARD, bot_move_failed, intermediate_path_found);
      }
    else
      {
	/* Stop the bot */
	asserv_stop_motor ();
	/* Post an event for the top FSM to be waked up later */
	main_move_wait_cycle = MOVE_WAIT_TIME_FOR_POOLING_SHARP;
	return move_next_branch (MOVING_BACKWARD, bot_move_failed, no_intermediate_path_found);
      }
}

/*
 * MOVING_BACKWARD =bot_move_succeed=>
 * intermediate_path_found => MOVING_TO_INTERMEDIATE_POSITION
 *   get next intermediate position from path module
 * no_intermediate_path_found => WAIT_FOR_CLEAR_PATH
 *   get next intermediate position from path module, failed
 *   stop the bot
 *   post an event for the top FSM to be waked up later
 */
fsm_branch_t
move__MOVING_BACKWARD__bot_move_succeed (void)
{
    /* Call generic function */
    if (move_after_moving_backward ())
      {
	return move_next_branch (MOVING_BACKWARD, bot_move_succeed, intermediate_path_found);
      }
    else
      {
	/* Stop the bot */
	asserv_stop_motor ();
	/* Post an event for the top FSM to be waked up later */
	main_move_wait_cycle = MOVE_WAIT_TIME_FOR_POOLING_SHARP;
	return move_next_branch (MOVING_BACKWARD, bot_move_succeed, no_intermediate_path_found);
      }
}

/*
 * MOVING_TO_FINAL_POSITION =bot_move_failed=>
 *  => MOVING_BACKWARD
 *   compute the obstacle position
 *   store current moving direction (for possible failed of path module)
 *   move backward to turn freely
 */
fsm_branch_t
move__MOVING_TO_FINAL_POSITION__bot_move_failed (void)
{
    /* Compute obstacle position */
    move_obstacle_here ();
    /* Store current moving direction */
    move_data.cached_moving_direction = asserv_get_moving_direction ();
    /* Move backward to turn freely */
    asserv_move_linearly (-PG_MOVE_DISTANCE);
    return move_next (MOVING_TO_FINAL_POSITION, bot_move_failed);
}

/*
 * MOVING_TO_FINAL_POSITION =bot_move_succeed=>
 *  => IDLE
 *   post an event for the top FSM to tell we have finished
 */
fsm_branch_t
move__MOVING_TO_FINAL_POSITION__bot_move_succeed (void)
{
    /* Post an event for the top FSM to tell we have finished */
    main_post_event_for_top_fsm = TOP_EVENT_move_fsm_finished;
    return move_next (MOVING_TO_FINAL_POSITION, bot_move_succeed);
}

/*
 * MOVING_TO_FINAL_POSITION =bot_move_obstacle=>
 * intermediate_path_found => MOVING_TO_INTERMEDIATE_POSITION
 *   compute the obstacle position
 *   get next intermediate position from path module
 *   go to next intermediate position
 * no_intermediate_path_found => WAIT_FOR_CLEAR_PATH
 *   compute the obstacle position
 *   get next intermediate position from path module failed
 *   store current moving direction
 *   stop the bot
 *   post an event for the top FSM to be waked up later
 */
fsm_branch_t
move__MOVING_TO_FINAL_POSITION__bot_move_obstacle (void)
{
    /* Compute obstacle position */
    move_obstacle_here ();
    /* Get next position */
    if (move_get_next_position (&move_data.intermediate))
      {
	/* Go to the next intermediate position */
	if (move_data.backward_movement_allowed)
	    asserv_goto_back (move_data.intermediate.x, move_data.intermediate.y);
	else
	    asserv_goto (move_data.intermediate.x, move_data.intermediate.y);
	return move_next_branch (MOVING_TO_FINAL_POSITION, bot_move_obstacle, intermediate_path_found);
      }
    else
      {
	/* Store current moving direction */
	move_data.cached_moving_direction = asserv_get_moving_direction ();
	/* Stop the bot */
	asserv_stop_motor ();
	/* Post an event for the top FSM to be waked up later */
	main_move_wait_cycle = MOVE_WAIT_TIME_FOR_POOLING_SHARP;
	return move_next_branch (MOVING_TO_FINAL_POSITION, bot_move_obstacle, no_intermediate_path_found);
      }
}

