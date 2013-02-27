/* move.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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

#define ANGFSM_NAME AI

#include "common.h"
#include "move.h"
#include "angfsm.h"
#include "bot.h"
#include "radar.h"
#include "asserv.h"
#include "main.h"
#include "events.h"
#include "playground_2010.h"
#include "loader.h"
#include "modules/path/path.h"
#include "modules/utils/utils.h"
#include "fsm_queue.h"
#include "modules/math/fixed/fixed.h"	/* fixed_* */
#include "modules/trace/trace.h"
#include "debug.host.h"
#include <math.h>

#define MOVE_LOADER_UNBLOCKING_DISTANCE 70

/** Used to define a grid.  If robot is outside the grid, be careful. */
#define MOVE_GRID_X 450
#define MOVE_GRID_X_ORIGIN 150
#define MOVE_GRID_Y 250
#define MOVE_GRID_Y_ORIGIN 128
#define MOVE_GRID_MARGIN 50

/**
 * Internal data used by the move FSM.
 */
struct move_data_t move_data;

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
    move_data.try_again_counter = 3;
    /* Start the FSM. */
    FSM_HANDLE (AI, move_start);
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
    move_data.try_again_counter = 3;
    /* Start the FSM. */
    FSM_HANDLE (AI, move_start);
}

void
move_obstacles_update (void)
{
    uint8_t i;
    for (i = 0; i < main_obstacles_nb; i++)
	path_obstacle (i, main_obstacles_pos[i], MOVE_OBSTACLE_RADIUS, 0,
		       MOVE_OBSTACLE_VALIDITY);
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

/* Define FSM */
FSM_STATES (
	    /* waiting for the beginning of the move FSM. */
	    MOVE_IDLE,
	    /* rotating towards next point. */
	    MOVE_ROTATING,
	    /* moving to a position (intermediate or final). */
	    MOVE_MOVING,
	    /* moving backward to go away from what is blocking the bot. */
	    MOVE_MOVING_BACKWARD_TO_TURN_FREELY,
	    /* waiting for obstacle to disappear. */
	    MOVE_WAIT_FOR_CLEAR_PATH,
	    /* moving loader up and move backward to unblock loader. */
	    MOVE_LOADER_UNBLOCKING_UPING,
	    /* moving loader down. */
	    MOVE_LOADER_UNBLOCKING_DOWNING)

FSM_EVENTS (
	    /* initialize the FSM and start the movement directly. */
	    move_start,
	    /* the bot has seen something (front is the same when going backward). */
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
	loader_down ();
	move_go ();
	return 1;
      }
    else
      {
	loader_up ();
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
    /* If not on the grid, slow down. */
    int16_t mx = current_pos.v.x % MOVE_GRID_X;
    int16_t my = current_pos.v.y % MOVE_GRID_Y;
    if (mx < MOVE_GRID_X_ORIGIN - MOVE_GRID_MARGIN
	|| mx > MOVE_GRID_X_ORIGIN + MOVE_GRID_MARGIN
	|| my < MOVE_GRID_Y_ORIGIN - MOVE_GRID_MARGIN
	|| my > MOVE_GRID_Y_ORIGIN + MOVE_GRID_MARGIN
       )
	move_data.slow = 1;
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
	   /* rotate towards next position. */
	   path_found_rotate, MOVE_ROTATING,
	   /* move to next position. */
	   path_found, MOVE_MOVING,
	   /* post failure event. */
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
	fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	return FSM_NEXT (MOVE_IDLE, move_start, no_path_found);
      }
}

/*
 * move to next position.
 */
FSM_TRANS (MOVE_ROTATING,
	   bot_move_succeed,
	   MOVE_MOVING)
{
    loader_down ();
    move_go ();
    return FSM_NEXT (MOVE_ROTATING, bot_move_succeed);
}

/*
 * move to next position.
 */
FSM_TRANS (MOVE_ROTATING,
	   bot_move_failed,
	   MOVE_MOVING)
{
    loader_down ();
    move_go ();
    return FSM_NEXT (MOVE_ROTATING, bot_move_failed);
}

/*
 * move to next position.
 */
FSM_TRANS_TIMEOUT (MOVE_ROTATING, 1125,
		   MOVE_MOVING)
{
    loader_down ();
    move_go ();
    return FSM_NEXT_TIMEOUT (MOVE_ROTATING);
}

/*
 * move backward
 * loader up
 */
FSM_TRANS (MOVE_ROTATING,
	   loader_errored,
	   MOVE_LOADER_UNBLOCKING_UPING)
{
    asserv_move_linearly (-MOVE_LOADER_UNBLOCKING_DISTANCE);
    loader_up ();
    return FSM_NEXT (MOVE_ROTATING, loader_errored);
}

FSM_TRANS (MOVE_MOVING, bot_move_succeed,
	   /* post success event. */
	   done, MOVE_IDLE,
	   /* rotate towards next position. */
	   path_found_rotate, MOVE_ROTATING,
	   /* move to next position. */
	   path_found, MOVE_MOVING,
	   /* post failure event */
	   no_path_found, MOVE_IDLE)
{
    if (move_data.final_move)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_fsm_succeed));
	return FSM_NEXT (MOVE_MOVING, bot_move_succeed, done);
      }
    else
      {
	uint8_t next = move_path_next ();
	if (next == 2)
	    return FSM_NEXT (MOVE_MOVING, bot_move_succeed, path_found_rotate);
	else
	    return FSM_NEXT (MOVE_MOVING, bot_move_succeed, path_found);
      }
    //return FSM_NEXT (MOVE_MOVING, bot_move_succeed, no_path_found);
}


/*
 * reset final_move.
 * move backward to turn freely.
 */
void
move_MOVE_MOVING_bot_move_failed_MOVE_MOVING_BACKWARD_TO_TURN_FREELY ()
{
    move_data.final_move = 0;
    /* Assume there is an obstacle in front of the robot. */
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    vect_t obstacle_pos;
    int16_t dist = asserv_get_last_moving_direction () == DIRECTION_FORWARD
	? BOT_SIZE_FRONT + MOVE_REAL_OBSTACLE_RADIUS
	: -(BOT_SIZE_BACK + MOVE_REAL_OBSTACLE_RADIUS);
    vect_from_polar_uf016 (&obstacle_pos, dist, robot_pos.a);
    vect_translate (&obstacle_pos, &robot_pos.v);
    path_obstacle (0, obstacle_pos, MOVE_OBSTACLE_RADIUS, 0,
		   MOVE_OBSTACLE_VALIDITY);
    /* Move backward to turn freely. */
    asserv_move_linearly (asserv_get_last_moving_direction ()
			  == DIRECTION_FORWARD ? -300 : 300);
}

FSM_TRANS (MOVE_MOVING,
	   bot_move_failed,
	   MOVE_MOVING_BACKWARD_TO_TURN_FREELY)
{
    move_MOVE_MOVING_bot_move_failed_MOVE_MOVING_BACKWARD_TO_TURN_FREELY ();
    return FSM_NEXT (MOVE_MOVING, bot_move_failed);
}

/*
 * reset final_move.
 * move backward to turn freely.
 */
FSM_TRANS_TIMEOUT (MOVE_MOVING, 2250,
		   MOVE_MOVING_BACKWARD_TO_TURN_FREELY)
{
    move_MOVE_MOVING_bot_move_failed_MOVE_MOVING_BACKWARD_TO_TURN_FREELY ();
    return FSM_NEXT_TIMEOUT (MOVE_MOVING);
}

FSM_TRANS (MOVE_MOVING, obstacle_in_front,
	   /* reset final_move.
	    * stop the bot. */
	   tryagain, MOVE_WAIT_FOR_CLEAR_PATH,
	   /* stop the bot.
	    * post failure event. */
	   tryout, MOVE_IDLE)
{
    move_data.final_move = 0;
    asserv_stop_motor ();
    if (--move_data.try_again_counter == 0)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	return FSM_NEXT (MOVE_MOVING, obstacle_in_front, tryout);
      }
    else
	return FSM_NEXT (MOVE_MOVING, obstacle_in_front, tryagain);
}

/*
 * move backward
 * loader up
 */
FSM_TRANS (MOVE_MOVING,
	   loader_errored,
	   MOVE_LOADER_UNBLOCKING_UPING)
{
    asserv_move_linearly (-MOVE_LOADER_UNBLOCKING_DISTANCE);
    loader_up ();
    return FSM_NEXT (MOVE_MOVING, loader_errored);
}

FSM_TRANS (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed,
	   /* post failure event. */
	   tryout, MOVE_IDLE,
	   /* rotate towards next position. */
	   path_found_rotate, MOVE_ROTATING,
	   /* move to next position. */
	   path_found, MOVE_MOVING,
	   /* post failure event. */
	   no_path_found, MOVE_IDLE)
{
    if (--move_data.try_again_counter == 0)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, tryout);
      }
    else
      {
	uint8_t next = move_path_init ();
	if (next)
	  {
	    if (next == 2)
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, path_found_rotate);
	    else
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, path_found);
	  }
	else
	  {
	    fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	    return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_succeed, no_path_found);
	  }
      }
}

FSM_TRANS (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed,
	   /* post failure event. */
	   tryout, MOVE_IDLE,
	   /* rotate towards next position. */
	   path_found_rotate, MOVE_ROTATING,
	   /* move to next position. */
	   path_found, MOVE_MOVING,
	   /* nothing to do. */
	   no_path_found_tryagain, MOVE_WAIT_FOR_CLEAR_PATH,
	   /* post failure event. */
	   no_path_found_tryout, MOVE_IDLE)
{
    if (--move_data.try_again_counter == 0)
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, tryout);
      }
    else
      {
	uint8_t next = move_path_init ();
	if (next)
	  {
	    if (next == 2)
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, path_found_rotate);
	    else
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, path_found);
	  }
	else
	  {
	    if (--move_data.try_again_counter == 0)
	      {
		fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, no_path_found_tryout);
	      }
	    else
		return FSM_NEXT (MOVE_MOVING_BACKWARD_TO_TURN_FREELY, bot_move_failed, no_path_found_tryagain);
	  }
      }
}

FSM_TRANS_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, 255,
		   /* rotate towards next position. */
		   path_found_rotate, MOVE_ROTATING,
		   /* move to next position. */
		   path_found, MOVE_MOVING,
		   /* decrement counter. */
		   no_path_found_tryagain, MOVE_WAIT_FOR_CLEAR_PATH,
		   /* post failure. */
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
	    fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	    return FSM_NEXT_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, no_path_found_tryout);
	  }
	else
	    return FSM_NEXT_TIMEOUT (MOVE_WAIT_FOR_CLEAR_PATH, no_path_found_tryagain);
      }
}

/*
 * loader down
 * reset unblocking retry counter
 */
FSM_TRANS (MOVE_LOADER_UNBLOCKING_UPING,
	   bot_move_succeed,
	   MOVE_LOADER_UNBLOCKING_DOWNING)
{
    loader_down ();
    move_data.loader_unblocking_retry = 2;
    return FSM_NEXT (MOVE_LOADER_UNBLOCKING_UPING, bot_move_succeed);
}

/*
 * loader down
 * reset unblocking retry counter
 */
FSM_TRANS (MOVE_LOADER_UNBLOCKING_UPING,
	   bot_move_failed,
	   MOVE_LOADER_UNBLOCKING_DOWNING)
{
    loader_down ();
    move_data.loader_unblocking_retry = 2;
    return FSM_NEXT (MOVE_LOADER_UNBLOCKING_UPING, bot_move_failed);
}

FSM_TRANS (MOVE_LOADER_UNBLOCKING_DOWNING, loader_downed,
	   /* rotate towards next position. */
	   path_found_rotate, MOVE_ROTATING,
	   /* move to next position. */
	   path_found, MOVE_MOVING,
	   /* post failure. */
	   no_path_found, MOVE_IDLE)
{
    /* Try to move. */
    uint8_t next = move_path_init ();
    if (next)
      {
	if (next == 2)
	    return FSM_NEXT (MOVE_LOADER_UNBLOCKING_DOWNING, loader_downed, path_found_rotate);
	else
	    return FSM_NEXT (MOVE_LOADER_UNBLOCKING_DOWNING, loader_downed, path_found);
      }
    else
      {
	fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	return FSM_NEXT (MOVE_LOADER_UNBLOCKING_DOWNING, loader_downed, no_path_found);
      }
}

FSM_TRANS (MOVE_LOADER_UNBLOCKING_DOWNING, loader_errored,
	   /* move backward.
	    * loader up. */
	   tryagain, MOVE_LOADER_UNBLOCKING_UPING,
	   /* rotate towards next position. */
	   tryout_path_found_rotate, MOVE_ROTATING,
	   /* move to next position. */
	   tryout_path_found, MOVE_ROTATING,
	   /* post failure. */
	   tryout_no_path_found, MOVE_IDLE)
{
    if (--move_data.loader_unblocking_retry)
      {
	asserv_move_linearly (-MOVE_LOADER_UNBLOCKING_DISTANCE);
	loader_up ();
	return FSM_NEXT (MOVE_LOADER_UNBLOCKING_DOWNING, loader_errored, tryagain);
      }
    else
      {
	/* Try to move. */
	uint8_t next = move_path_init ();
	if (next)
	  {
	    if (next == 2)
		return FSM_NEXT (MOVE_LOADER_UNBLOCKING_DOWNING, loader_errored, tryout_path_found_rotate);
	    else
		return FSM_NEXT (MOVE_LOADER_UNBLOCKING_DOWNING, loader_errored, tryout_path_found);
	  }
	else
	  {
	    fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	    return FSM_NEXT (MOVE_LOADER_UNBLOCKING_DOWNING, loader_errored, tryout_no_path_found);
	  }
      }
}

FSM_TRANS_TIMEOUT (MOVE_LOADER_UNBLOCKING_DOWNING, 450,
		   /* move backward.
		    * loader up. */
		   tryagain, MOVE_LOADER_UNBLOCKING_UPING,
		   /* rotate towards next position. */
		   tryout_path_found_rotate, MOVE_ROTATING,
		   /* move to next position. */
		   tryout_path_found, MOVE_ROTATING,
		   /* post failure. */
		   tryout_no_path_found, MOVE_IDLE)
{
    if (--move_data.loader_unblocking_retry)
      {
	asserv_move_linearly (-MOVE_LOADER_UNBLOCKING_DISTANCE);
	loader_up ();
	return FSM_NEXT_TIMEOUT (MOVE_LOADER_UNBLOCKING_DOWNING, tryagain);
      }
    else
      {
	/* Try to move. */
	uint8_t next = move_path_init ();
	if (next)
	  {
	    if (next == 2)
		return FSM_NEXT_TIMEOUT (MOVE_LOADER_UNBLOCKING_DOWNING, tryout_path_found_rotate);
	    else
		return FSM_NEXT_TIMEOUT (MOVE_LOADER_UNBLOCKING_DOWNING, tryout_path_found);
	  }
	else
	  {
	    fsm_queue_post_event (FSM_EVENT (AI, move_fsm_failed));
	    return FSM_NEXT_TIMEOUT (MOVE_LOADER_UNBLOCKING_DOWNING, tryout_no_path_found);
	  }
      }
}

