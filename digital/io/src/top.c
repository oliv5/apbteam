/* top.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Haller
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

#define FSM_NAME AI

#include "fsm.h"
#include "common.h"
#include "defs.h"
#include "top.h"
#include "food.h"
#include "asserv.h"
#include "loader.h"
#include "move.h"
#include "chrono.h"
#include "playground_2010.h"
#include "bot.h"

#include "modules/utils/utils.h"

uint8_t top_food;

/** Maximum elements to load before unloading. */
#define TOP_LOADER_MAX 3
/** Maximum elements to load before unloading when far from unload area. */
#define TOP_LOADER_MAX_FAR 4
/** Time to reserve for unloading at end of round. */
#define TOP_TIME_LIMIT_MS 20000ll

void
top_init (void)
{
}

uint8_t
top_collect (uint8_t force)
{
    uint8_t loader_max;
    position_t robot_position;
    asserv_get_position (&robot_position);
    if (UTILS_ABS (PG_X (PG_WIDTH) - robot_position.v.x) < PG_WIDTH / 2)
	loader_max = TOP_LOADER_MAX;
    else
	loader_max = TOP_LOADER_MAX_FAR;
    if ((loader_elements < loader_max
	 && (loader_elements == 0
	     || chrono_remaining_time () > TOP_TIME_LIMIT_MS))
	|| force)
      {
	top_food = food_best (robot_position);
	if (top_food == 0xff)
	    return 0;
	vect_t food_v;
	food_pos (top_food, &food_v);
	/* If loaded an next ball is far away, go unload. */
	if (loader_elements
	    && UTILS_ABS (food_v.x - robot_position.v.x) > 1000)
	  {
	    asserv_set_speed (BOT_MOVE_FAST);
	    move_start_noangle (PG_VECT (2625, 253), 0, 0);
	    return 0;
	  }
	asserv_set_speed (BOT_MOVE_MEDIUM);
	move_start_noangle (food_v, 0, food_shorten (top_food));
	return 1;
      }
    else
      {
	asserv_set_speed (BOT_MOVE_FAST);
	move_start_noangle (PG_VECT (2625, 253), 0, 0);
	return 0;
      }
}

FSM_INIT

FSM_STATES (IDLE,
	    WAIT_INIT_TO_FINISH,
	    FIRST_GO_BEGIN_OF_LINE_FAST,
	    FIRST_GO_END_OF_LINE_FAST,
	    FIRST_GO_END_OF_LINE_SLOW,
	    UNLOAD,
	    UNLOAD_LOADER_UP,
	    UNLOAD_FACE_BIN,
	    UNLOAD_FACE_BIN_UNBLOCK,
	    UNLOAD_BACK_BIN,
	    UNLOAD_UNLOAD,
	    COLLECT,
	    COLLECT_SLOW_MOTION,
	    COLLECT_BLACK)

FSM_EVENTS (start,
	    move_fsm_succeed,
	    move_fsm_failed,
	    bot_move_succeed,
	    bot_move_failed,
	    init_match_is_started,
	    in_field,
	    asserv_last_cmd_ack,
	    clamp_succeed)

FSM_START_WITH (IDLE)

/* Nothing to do. */
FSM_TRANS (IDLE, start, WAIT_INIT_TO_FINISH)
{
    return FSM_NEXT (IDLE, start);
}

/*
 * the match start
 * set fast speed
 * go to first sequence begin of line
 */
FSM_TRANS (WAIT_INIT_TO_FINISH,
	   init_match_is_started,
	   FIRST_GO_BEGIN_OF_LINE_FAST)
{
    asserv_set_speed (BOT_MOVE_FAST);
    move_start_noangle (PG_VECT (375, 1503), 0, 0);
    return FSM_NEXT (WAIT_INIT_TO_FINISH, init_match_is_started);
}

/*
 * go to end of line
 * loader down
 */
FSM_TRANS (FIRST_GO_BEGIN_OF_LINE_FAST,
	   move_fsm_succeed,
	   FIRST_GO_END_OF_LINE_FAST)
{
    move_start_noangle (PG_VECT (2625, 253), 0, 0);
    loader_down ();
    return FSM_NEXT (FIRST_GO_BEGIN_OF_LINE_FAST, move_fsm_succeed);
}

/*
 * set slow speed
 * retry
 */
FSM_TRANS (FIRST_GO_BEGIN_OF_LINE_FAST,
	   move_fsm_failed,
	   FIRST_GO_BEGIN_OF_LINE_FAST)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    move_start_noangle (PG_VECT (375, 1503), 0, 0);
    return FSM_NEXT (FIRST_GO_BEGIN_OF_LINE_FAST, move_fsm_failed);
}

/*
 * set slow speed
 */
FSM_TRANS (FIRST_GO_END_OF_LINE_FAST,
	   in_field,
	   FIRST_GO_END_OF_LINE_SLOW)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    return FSM_NEXT (FIRST_GO_END_OF_LINE_FAST, in_field);
}

/*
 * set slow speed
 * move loader up
 */
FSM_TRANS (FIRST_GO_END_OF_LINE_FAST,
	   move_fsm_succeed,
	   UNLOAD_LOADER_UP)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    loader_up ();
    return FSM_NEXT (FIRST_GO_END_OF_LINE_FAST, move_fsm_succeed);
}

/*
 * set slow speed
 * retry
 */
FSM_TRANS (FIRST_GO_END_OF_LINE_FAST,
	   move_fsm_failed,
	   FIRST_GO_END_OF_LINE_SLOW)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    move_start_noangle (PG_VECT (2625, 253), 0, 0);
    return FSM_NEXT (FIRST_GO_END_OF_LINE_FAST, move_fsm_failed);
}

/*
 * move loader up
 */
FSM_TRANS (FIRST_GO_END_OF_LINE_SLOW,
	   move_fsm_succeed,
	   UNLOAD_LOADER_UP)
{
    loader_up ();
    return FSM_NEXT (FIRST_GO_END_OF_LINE_SLOW, move_fsm_succeed);
}

/*
 * retry
 */
FSM_TRANS (FIRST_GO_END_OF_LINE_SLOW,
	   move_fsm_failed,
	   FIRST_GO_END_OF_LINE_SLOW)
{
    move_start_noangle (PG_VECT (2625, 253), 0, 0);
    return FSM_NEXT (FIRST_GO_END_OF_LINE_SLOW, move_fsm_failed);
}

/*
 * move loader up
 */
FSM_TRANS (UNLOAD, move_fsm_succeed, UNLOAD_LOADER_UP)
{
    loader_up ();
    return FSM_NEXT (UNLOAD, move_fsm_succeed);
}

/*
 * retry
 */
FSM_TRANS (UNLOAD, move_fsm_failed, UNLOAD)
{
    move_start_noangle (PG_VECT (2625, 253), 0, 0);
    return FSM_NEXT (UNLOAD, move_fsm_failed);
}

/*
 * turn toward bin
 */
FSM_TRANS (UNLOAD_LOADER_UP, loader_uped, UNLOAD_FACE_BIN)
{
    asserv_goto_angle (PG_A_DEG (90));
    return FSM_NEXT (UNLOAD_LOADER_UP, loader_uped);
}

/*
 * turn toward bin
 */
FSM_TRANS (UNLOAD_LOADER_UP, loader_errored, UNLOAD_FACE_BIN)
{
    asserv_goto_angle (PG_A_DEG (90));
    return FSM_NEXT (UNLOAD_LOADER_UP, loader_errored);
}

/*
 * go backward to bin
 */
FSM_TRANS (UNLOAD_FACE_BIN, bot_move_succeed, UNLOAD_BACK_BIN)
{
    asserv_move_linearly (-(128 + 250 / 2 - BOT_SIZE_BACK - 50));
    return FSM_NEXT (UNLOAD_FACE_BIN, bot_move_succeed);
}

/*
 * move backward
 */
FSM_TRANS (UNLOAD_FACE_BIN, bot_move_failed, UNLOAD_FACE_BIN_UNBLOCK)
{
    asserv_move_linearly (-40);
    return FSM_NEXT (UNLOAD_FACE_BIN, bot_move_failed);
}

/*
 * turn toward bin
 */
FSM_TRANS (UNLOAD_FACE_BIN_UNBLOCK, bot_move_succeed, UNLOAD_FACE_BIN)
{
    asserv_goto_angle (PG_A_DEG (90));
    return FSM_NEXT (UNLOAD_FACE_BIN_UNBLOCK, bot_move_succeed);
}

/*
 * turn toward bin
 */
FSM_TRANS (UNLOAD_FACE_BIN_UNBLOCK, bot_move_failed, UNLOAD_FACE_BIN)
{
    asserv_goto_angle (PG_A_DEG (90));
    return FSM_NEXT (UNLOAD_FACE_BIN_UNBLOCK, bot_move_failed);
}

/*
 * unload
 */
FSM_TRANS (UNLOAD_BACK_BIN, bot_move_succeed, UNLOAD_UNLOAD)
{
    asserv_move_motor1_absolute (0, BOT_GATE_SPEED);
    return FSM_NEXT (UNLOAD_BACK_BIN, bot_move_succeed);
}

/*
 * unload
 */
FSM_TRANS (UNLOAD_BACK_BIN, bot_move_failed, UNLOAD_UNLOAD)
{
    asserv_move_motor1_absolute (0, BOT_GATE_SPEED);
    return FSM_NEXT (UNLOAD_BACK_BIN, bot_move_failed);
}

/*
 * close gate
 * loader down
 * choose best food to collect
 */
FSM_TRANS_TIMEOUT (UNLOAD_UNLOAD, 255, COLLECT)
{
    loader_elements = 0;
    asserv_move_motor1_absolute (BOT_GATE_STROKE_STEP, BOT_GATE_SPEED);
    loader_down ();
    top_collect (1);
    return FSM_NEXT_TIMEOUT (UNLOAD_UNLOAD);
}

/*
 * post loader_element event
 */
FSM_TRANS (COLLECT, move_fsm_succeed,
	   unload, UNLOAD,
	   collect, COLLECT,
	   slow_motion, COLLECT_SLOW_MOTION)
{
    int16_t slow_motion = food_slow_motion (top_food);
    if (slow_motion)
      {
	FSM_HANDLE (AI, loader_element);
	return FSM_NEXT (COLLECT, move_fsm_succeed, slow_motion);
      }
    else if (top_collect (0))
	return FSM_NEXT (COLLECT, move_fsm_succeed, collect);
    else
	return FSM_NEXT (COLLECT, move_fsm_succeed, unload);
}

/*
 * cancel this element
 */
FSM_TRANS (COLLECT, move_fsm_failed,
	   unload, UNLOAD,
	   collect, COLLECT)
{
    if (top_collect (0))
      {
	food_cancel (top_food);
	return FSM_NEXT (COLLECT, move_fsm_failed, collect);
      }
    else
	return FSM_NEXT (COLLECT, move_fsm_failed, unload);
}

/*
 * move backward
 * mark as black
 */
FSM_TRANS (COLLECT, loader_black, COLLECT_BLACK)
{
    asserv_move_linearly (-90);
    food_black (top_food);
    return FSM_NEXT (COLLECT, loader_black);
}

/*
 * collect
 */
FSM_TRANS (COLLECT_SLOW_MOTION, loader_downed,
	   unload, UNLOAD,
	   collect, COLLECT)
{
    if (top_collect (0))
	return FSM_NEXT (COLLECT_SLOW_MOTION, loader_downed, collect);
    else
	return FSM_NEXT (COLLECT_SLOW_MOTION, loader_downed, unload);
}

/*
 * same as above
 */
FSM_TRANS (COLLECT_SLOW_MOTION, loader_errored,
	   unload, UNLOAD,
	   collect, COLLECT)
{
    if (top_collect (0))
	return FSM_NEXT (COLLECT_SLOW_MOTION, loader_errored, collect);
    else
	return FSM_NEXT (COLLECT_SLOW_MOTION, loader_errored, unload);
}

/*
 * same as above
 */
FSM_TRANS_TIMEOUT (COLLECT_SLOW_MOTION, 450,
		   unload, UNLOAD,
		   collect, COLLECT)
{
    if (top_collect (0))
	return FSM_NEXT_TIMEOUT (COLLECT_SLOW_MOTION, collect);
    else
	return FSM_NEXT_TIMEOUT (COLLECT_SLOW_MOTION, unload);
}

/*
 * move backward
 * mark as black
 */
FSM_TRANS (COLLECT_SLOW_MOTION, loader_black, COLLECT_BLACK)
{
    asserv_move_linearly (-90);
    food_black (top_food);
    return FSM_NEXT (COLLECT_SLOW_MOTION, loader_black);
}

FSM_TRANS (COLLECT_BLACK, bot_move_succeed,
	   unload, UNLOAD,
	   collect, COLLECT)
{
    if (top_collect (0))
	return FSM_NEXT (COLLECT_BLACK, bot_move_succeed, collect);
    else
	return FSM_NEXT (COLLECT_BLACK, bot_move_succeed, unload);
}

FSM_TRANS (COLLECT_BLACK, bot_move_failed,
	   unload, UNLOAD,
	   collect, COLLECT)
{
    if (top_collect (0))
	return FSM_NEXT (COLLECT_BLACK, bot_move_failed, collect);
    else
	return FSM_NEXT (COLLECT_BLACK, bot_move_failed, unload);
}

