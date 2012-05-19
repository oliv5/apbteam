/* move.c */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "move.h"

#include "main.h"
#include "asserv.h"

#define FSM_NAME AI
#include "fsm.h"
#include "fsm_queue.h"

#include "radar.h"
#include "path.h"

#include "modules/utils/utils.h"
#include "modules/proto/proto.h"

#include <math.h>

#include "playground_2012.h"

/** Number of tries to reach destination. */
#define MOVE_TRY_AGAIN 1

/** Move context. */
struct move_t
{
    /** Final position. */
    position_t final;
    /** Use angle consign for final point. */
    uint8_t with_angle;
    /** Next step. */
    vect_t step;
    /** Next step angle. */
    uint16_t step_angle;
    /** Next step with_angle. */
    uint8_t step_with_angle;
    /** Next step backward. */
    uint8_t step_backward;
    /** Non zero means this is a tricky move, slow down, and minimize
     * turns. */
    uint8_t slow;
    /** Backward direction allowed flag. */
    uint8_t backward_movement_allowed;
    /** Try again counter. */
    uint8_t try_again_counter;
    /** Dirty fix to know this is the final move. */
    uint8_t final_move;
    /** Distance to remove from path. */
    int16_t shorten;
};

/* Global context. */
struct move_t move_data;

void
move_start (position_t position, uint8_t backward)
{
    /* Set parameters. */
    move_data.final = position;
    move_data.with_angle = 1;
    move_data.backward_movement_allowed = backward;
    move_data.final_move = 0;
    move_data.shorten = 0;
    /* Reset try counter. */
    move_data.try_again_counter = MOVE_TRY_AGAIN;
    /* Start the FSM. */
    fsm_queue_post_event (FSM_EVENT (AI, move_start));
}

void
move_start_noangle (vect_t position, uint8_t backward, int16_t shorten)
{
    /* Set parameters. */
    move_data.final.v = position;
    move_data.with_angle = 0;
    move_data.backward_movement_allowed = backward;
    move_data.final_move = 0;
    move_data.shorten = shorten;
    /* Reset try counter. */
    move_data.try_again_counter = MOVE_TRY_AGAIN;
    /* Start the FSM. */
    fsm_queue_post_event (FSM_EVENT (AI, move_start));
}

void
move_stop (void)
{
    /* Stop the FSM. */
    fsm_queue_post_event (FSM_EVENT (AI, move_stop));
}

void
move_obstacles_update (void)
{
    uint8_t i;
    for (i = 0; i < main_obstacles_nb; i++)
      {
	path_obstacle (i, main_obstacles_pos[i], MOVE_OBSTACLE_RADIUS, 0,
		       MOVE_OBSTACLE_VALIDITY);
	proto_send2w ('o', main_obstacles_pos[i].x,
		      main_obstacles_pos[i].y);
      }
}

uint8_t
move_check_obstacles (void)
{
    if (FSM_CAN_HANDLE (AI, obstacle_in_front))
      {
	position_t robot_pos;
	asserv_get_position (&robot_pos);
	if (radar_blocking (&robot_pos.v, &move_data.step, main_obstacles_pos,
			    main_obstacles_nb))
	    if (FSM_HANDLE (AI, obstacle_in_front))
		return 1;
      }
    return 0;
}

FSM_STATES (
	    /* Waiting for the start order. */
	    MOVE_IDLE,
	    /* Rotating towards next point. */
	    MOVE_ROTATING,
	    /* Moving to a position (intermediate or final). */
	    MOVE_MOVING,
	    /* Brake when a obstacle is seen. */
	    MOVE_BRAKE,
	    /* Moving backward to go away from what is blocking the bot. */
	    MOVE_MOVING_BACKWARD_TO_TURN_FREELY,
	    /* Waiting for obstacle to disappear. */
	    MOVE_WAIT_FOR_CLEAR_PATH)

FSM_EVENTS (
	    /* Report from asserv after a successful move command. */
	    robot_move_success,
	    /* Report from asserv after a failed move command. */
	    robot_move_failure,
	    /* Initialize the FSM and start the movement directly. */
	    move_start,
	    /* Stop movement. */
	    move_stop,
	    /* Movement success. */
	    move_success,
	    /* Movement failure. */
	    move_failure,
	    /* The bot has seen something (front is the same when going backward). */
	    obstacle_in_front)

FSM_START_WITH (MOVE_IDLE)

/** Go to current step, low level function. */
static void
move_go (void)
{
    vect_t dst = move_data.step;
    /* Modify final point if requested. */
    if (move_data.final_move && move_data.shorten)
      {
	/* Compute a vector from destination to robot with lenght
	 * 'shorten'. */
	position_t robot_position;
	asserv_get_position (&robot_position);
	vect_t v = robot_position.v;
	vect_sub (&v, &move_data.step);
	int16_t d = vect_norm (&v);
	if (d > move_data.shorten)
	  {
	    vect_scale_f824 (&v, 0x1000000 / d * move_data.shorten);
	    vect_translate (&dst, &v);
	  }
      }
    if (move_data.step_with_angle)
	asserv_goto_xya (dst.x, dst.y, move_data.step_angle,
			 move_data.step_backward);
    else
	asserv_goto (dst.x, dst.y, move_data.step_backward);
}

/** Go or rotate toward position, returns 1 for linear move, 2 for angular
 * move. */
static uint8_t
move_go_or_rotate (vect_t dst, uint16_t angle, uint8_t with_angle,
		   uint8_t backward)
{
    position_t robot_position;
    asserv_get_position (&robot_position);
    uint16_t robot_angle = robot_position.a;
    if (backward & ASSERV_BACKWARD)
	robot_angle += 0x8000;
    /* Remember step. */
    move_data.step = dst;
    move_data.step_angle = angle;
    move_data.step_with_angle = with_angle;
    move_data.step_backward = backward;
    /* Compute angle to destination. */
    vect_t v = dst; vect_sub (&v, &robot_position.v);
    uint16_t dst_angle = atan2 (v.y, v.x) * ((1l << 16) / (2 * M_PI));
    if (backward & ASSERV_BACKWARD)
	dst_angle += 0x8000;
    int16_t diff_angle = dst_angle - robot_angle;
    if ((backward & ASSERV_REVERT_OK)
	&& (diff_angle > 0x4000 || diff_angle < -0x4000))
	dst_angle += 0x8000;
    int16_t diff = dst_angle - robot_angle;
    /* Move or rotate. */
    if (UTILS_ABS (diff) < 0x1000)
      {
	move_go ();
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

FSM_TRANS (MOVE_IDLE, move_start,
	   path_found_rotate, MOVE_ROTATING,
	   path_found, MOVE_MOVING,
	   no_path_found, MOVE_IDLE)
{
    uint8_t next = move_path_init ();
    if (next)
      {
	if (next == 2)
	    return FSM_NEXT (MOVE_IDLE, move_start, path_found_rotate);
	else
	    return FSM_NEXT (MOVE_IDLE, move_start, path_found);
      }
    else
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_failure));
	return FSM_NEXT (MOVE_IDLE, move_start, no_path_found);
      }
}

FSM_TRANS (MOVE_ROTATING,
	   robot_move_success,
	   MOVE_MOVING)
{
    move_go ();
    return FSM_NEXT (MOVE_ROTATING, robot_move_success);
}

FSM_TRANS (MOVE_ROTATING,
	   robot_move_failure,
	   MOVE_MOVING)
{
    move_go ();
    return FSM_NEXT (MOVE_ROTATING, robot_move_failure);
}

FSM_TRANS_TIMEOUT (MOVE_ROTATING, 1250,
		   MOVE_MOVING)
{
    move_go ();
    return FSM_NEXT_TIMEOUT (MOVE_ROTATING);
}

FSM_TRANS (MOVE_ROTATING, move_stop, MOVE_IDLE)
{
    asserv_stop_motor ();
    return FSM_NEXT (MOVE_ROTATING, move_stop);
}

FSM_TRANS (MOVE_MOVING, robot_move_success,
	   done, MOVE_IDLE,
	   path_found_rotate, MOVE_ROTATING,
	   path_found, MOVE_MOVING,
	   no_path_found, MOVE_IDLE)
{
    if (move_data.final_move)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_success));
	return FSM_NEXT (MOVE_MOVING, robot_move_success, done);
      }
    else
      {
	uint8_t next = move_path_next ();
	if (next == 2)
	    return FSM_NEXT (MOVE_MOVING, robot_move_success, path_found_rotate);
	else
	    return FSM_NEXT (MOVE_MOVING, robot_move_success, path_found);
      }
    //return FSM_NEXT (MOVE_MOVING, robot_move_success, no_path_found);
}

/** Test if a point is ok to move back. */
static uint8_t
move_test_point (vect_t p, int16_t margin)
{
    return p.x >= margin && p.x < PG_WIDTH - margin
	&& p.y >= margin && p.y < PG_LENGTH - margin
	&& !(p.y >= PG_TOTEM_Y - PG_TOTEM_WIDTH_MM / 2 - margin
	     && p.y < PG_TOTEM_Y + PG_TOTEM_WIDTH_MM / 2 + margin
	     && p.x >= PG_TOTEM_LEFT_X - PG_TOTEM_WIDTH_MM / 2 - margin
	     && p.x < PG_TOTEM_RIGHT_X + PG_TOTEM_WIDTH_MM / 2 + margin);
}

static void
move_moving_backward_to_turn_freely (void)
{
    move_data.final_move = 0;
    int16_t dist, back_dist, margin;
    if (asserv_get_last_moving_direction () == DIRECTION_FORWARD)
      {
	dist = BOT_SIZE_FRONT + MOVE_REAL_OBSTACLE_RADIUS;
	back_dist = -300;
	margin = BOT_SIZE_BACK_RADIUS;
      }
    else
      {
	dist = -(BOT_SIZE_FRONT + MOVE_REAL_OBSTACLE_RADIUS);
	back_dist = 300;
	margin = BOT_SIZE_RADIUS;
      }
    /* Assume there is an obstacle in front of the robot. */
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    vect_t obstacle_pos;
    vect_from_polar_uf016 (&obstacle_pos, dist, robot_pos.a);
    vect_translate (&obstacle_pos, &robot_pos.v);
    path_obstacle (0, obstacle_pos, MOVE_OBSTACLE_RADIUS, 0,
		   MOVE_OBSTACLE_VALIDITY);
    /* Move backward to turn freely. */
    vect_t back_pos;
    vect_from_polar_uf016 (&back_pos, back_dist, robot_pos.a);
    vect_translate (&back_pos, &robot_pos.v);
    if (!move_test_point (back_pos, margin))
	back_dist /= 8;
    asserv_move_linearly (back_dist);
}

FSM_TRANS (MOVE_MOVING,
	   robot_move_failure,
	   MOVE_MOVING_BACKWARD_TO_TURN_FREELY)
{
    move_moving_backward_to_turn_freely ();
    return FSM_NEXT (MOVE_MOVING, robot_move_failure);
}

FSM_TRANS_TIMEOUT (MOVE_MOVING, 2500,
		   MOVE_MOVING_BACKWARD_TO_TURN_FREELY)
{
    move_moving_backward_to_turn_freely ();
    return FSM_NEXT_TIMEOUT (MOVE_MOVING);
}

FSM_TRANS (MOVE_MOVING, obstacle_in_front, MOVE_BRAKE)
{
    move_data.final_move = 0;
    asserv_stop_motor ();
    return FSM_NEXT (MOVE_MOVING, obstacle_in_front);
}

FSM_TRANS (MOVE_BRAKE, robot_move_success,
	   tryagain, MOVE_WAIT_FOR_CLEAR_PATH,
	   tryout, MOVE_IDLE)
{
    if (--move_data.try_again_counter == 0)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_failure));
	return FSM_NEXT (MOVE_BRAKE, robot_move_success, tryout);
      }
    else
	return FSM_NEXT (MOVE_BRAKE, robot_move_success, tryagain);
}

FSM_TRANS (MOVE_BRAKE, robot_move_failure,
	   tryagain, MOVE_WAIT_FOR_CLEAR_PATH,
	   tryout, MOVE_IDLE)
{
    if (--move_data.try_again_counter == 0)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_failure));
	return FSM_NEXT (MOVE_BRAKE, robot_move_failure, tryout);
      }
    else
	return FSM_NEXT (MOVE_BRAKE, robot_move_failure, tryagain);
}

FSM_TRANS (MOVE_MOVING, move_stop, MOVE_IDLE)
{
    asserv_stop_motor ();
    return FSM_NEXT (MOVE_MOVING, move_stop);
}

FSM_TRANS (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_success,
	   tryout, MOVE_IDLE,
	   path_found_rotate, MOVE_ROTATING,
	   path_found, MOVE_MOVING,
	   no_path_found, MOVE_IDLE)
{
    if (--move_data.try_again_counter == 0)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_failure));
	return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_success, tryout);
      }
    else
      {
	uint8_t next = move_path_init ();
	if (next)
	  {
	    if (next == 2)
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_success, path_found_rotate);
	    else
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_success, path_found);
	  }
	else
	  {
	    fsm_queue_post_event (FSM_EVENT (AI, move_failure));
	    return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_success, no_path_found);
	  }
      }
}

FSM_TRANS (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_failure,
	   tryout, MOVE_IDLE,
	   path_found_rotate, MOVE_ROTATING,
	   path_found, MOVE_MOVING,
	   no_path_found_tryagain, MOVE_WAIT_FOR_CLEAR_PATH,
	   no_path_found_tryout, MOVE_IDLE)
{
    if (--move_data.try_again_counter == 0)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_failure));
	return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_failure, tryout);
      }
    else
      {
	uint8_t next = move_path_init ();
	if (next)
	  {
	    if (next == 2)
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_failure, path_found_rotate);
	    else
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_failure, path_found);
	  }
	else
	  {
	    if (--move_data.try_again_counter == 0)
	      {
		fsm_queue_post_event (FSM_EVENT (AI, move_failure));
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_failure, no_path_found_tryout);
	      }
	    else
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, robot_move_failure, no_path_found_tryagain);
	  }
      }
}

FSM_TRANS (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, move_stop, MOVE_IDLE)
{
    asserv_stop_motor ();
    return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, move_stop);
}

FSM_TRANS_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, 250,
		   path_found_rotate, MOVE_ROTATING,
		   path_found, MOVE_MOVING,
		   no_path_found_tryagain, MOVE_WAIT_FOR_CLEAR_PATH,
		   no_path_found_tryout, MOVE_IDLE)
{
    /* Try to move. */
    uint8_t next = move_path_init ();
    if (next)
      {
	if (next == 2)
	    return FSM_NEXT_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, path_found_rotate);
	else
	    return FSM_NEXT_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, path_found);
      }
    else
      {
	/* Error, no new position, should we try again? */
	if (--move_data.try_again_counter == 0)
	  {
	    fsm_queue_post_event (FSM_EVENT (AI, move_failure));
	    return FSM_NEXT_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, no_path_found_tryout);
	  }
	else
	    return FSM_NEXT_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, no_path_found_tryagain);
      }
}

FSM_TRANS (MOVE_WAIT_FOR_CLEAR_PATH, move_stop, MOVE_IDLE)
{
    return FSM_NEXT (MOVE_WAIT_FOR_CLEAR_PATH, move_stop);
}

