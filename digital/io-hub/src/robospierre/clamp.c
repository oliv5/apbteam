/* clamp.c */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "clamp.h"

#include "mimot.h"
#include "pwm.h"
#include "contact.h"
#include "bot.h"
#include "element.h"

#define FSM_NAME AI
#include "fsm.h"
#include "fsm_queue.h"

#include "logistic.h"

/*
 * There is two FSM in this file.
 *
 * The clamp FSM handles high level clamp behaviour, new elements, drop, and
 * gives orders to the clamp move FSM.
 *
 * The clamp move FSM only handle moving the clamp without load or moving an
 * element from a slot to another one.
 */

FSM_INIT

FSM_STATES (
	    /* Initial state, to be complete with full initialisation. */
	    CLAMP_START,
	    /* Returning to idle position. */
	    CLAMP_GOING_IDLE,
	    /* Waiting external events, clamp at middle level. */
	    CLAMP_IDLE,
	    /* Taking an element at bottom slots. */
	    CLAMP_TAKING_DOOR_CLOSING,
	    /* Moving elements around. */
	    CLAMP_MOVING_ELEMENT,

	    /* Waiting movement order. */
	    CLAMP_MOVE_IDLE,
	    /* Moving to a final or intermediary position. */
	    CLAMP_MOVE_ROUTING,
	    /* Moving to source slot. */
	    CLAMP_MOVE_SRC_ROUTING,
	    /* Closing the clamp once arrived at source. */
	    CLAMP_MOVE_SRC_CLAMP_CLOSING,
	    /* Opening door once clamp closed. */
	    CLAMP_MOVE_SRC_DOOR_OPENDING,
	    /* Moving to destination slot. */
	    CLAMP_MOVE_DST_ROUTING,
	    /* Closing door once arrived at destination. */
	    CLAMP_MOVE_DST_DOOR_CLOSING,
	    /* Opening the clamp once door closed. */
	    CLAMP_MOVE_DST_CLAMP_OPENING)

FSM_EVENTS (
	    /* Here for the moment, to be moved later. */
	    start,
	    /* New element inside bottom slot. */
	    clamp_new_element,
	    /* Order to move the clamp. */
	    clamp_move,
	    /* Clamp movement success. */
	    clamp_move_success,
	    /* Elevation and elevation motor success. */
	    clamp_elevation_rotation_success,
	    /* Elevation motor failure. */
	    clamp_elevation_failure,
	    /* Rotation motor failure. */
	    clamp_rotation_failure)

FSM_START_WITH (CLAMP_START)
FSM_START_WITH (CLAMP_MOVE_IDLE)

/** Clamp context. */
struct clamp_t
{
    /** Current position. */
    uint8_t pos_current;
    /** Requested position. */
    uint8_t pos_request;
    /** Element moving destination. */
    uint8_t moving_to;
    /** Position of a new element. */
    uint8_t pos_new;
    /** New element kind. */
    uint8_t new_element;
};

/** Global context. */
struct clamp_t clamp_global;
#define ctx clamp_global

/** Clamp positions. */
static const uint16_t clamp_pos[][2] = {
      { BOT_CLAMP_SLOT_FRONT_BOTTOM_ELEVATION_STEP,
	BOT_CLAMP_BAY_FRONT_ROTATION_STEP },
      { BOT_CLAMP_SLOT_FRONT_MIDDLE_ELEVATION_STEP,
	BOT_CLAMP_BAY_FRONT_ROTATION_STEP },
      { BOT_CLAMP_SLOT_FRONT_TOP_ELEVATION_STEP,
	BOT_CLAMP_BAY_FRONT_ROTATION_STEP },
      { BOT_CLAMP_SLOT_BACK_BOTTOM_ELEVATION_STEP,
	BOT_CLAMP_BAY_BACK_ROTATION_STEP },
      { BOT_CLAMP_SLOT_BACK_MIDDLE_ELEVATION_STEP,
	BOT_CLAMP_BAY_BACK_ROTATION_STEP },
      { BOT_CLAMP_SLOT_BACK_TOP_ELEVATION_STEP,
	BOT_CLAMP_BAY_BACK_ROTATION_STEP },
      { BOT_CLAMP_SLOT_SIDE_ELEVATION_STEP,
	BOT_CLAMP_BAY_SIDE_ROTATION_STEP },
      { BOT_CLAMP_BAY_FRONT_LEAVE_ELEVATION_STEP,
	BOT_CLAMP_BAY_FRONT_ROTATION_STEP },
      { BOT_CLAMP_BAY_BACK_LEAVE_ELEVATION_STEP,
	BOT_CLAMP_BAY_BACK_ROTATION_STEP },
      { BOT_CLAMP_BAY_SIDE_ENTER_LEAVE_ELEVATION_STEP,
	BOT_CLAMP_BAY_SIDE_ROTATION_STEP },
};

/** Slot doors. */
static const uint8_t clamp_slot_door[] = {
    BOT_PWM_DOOR_FRONT_BOTTOM,
    0xff,
    BOT_PWM_DOOR_FRONT_TOP,
    BOT_PWM_DOOR_BACK_BOTTOM,
    0xff,
    BOT_PWM_DOOR_BACK_TOP,
    0xff
};

void
clamp_move (uint8_t pos)
{
    if (pos != ctx.pos_current)
      {
	ctx.pos_request = pos;
	ctx.moving_to = CLAMP_POS_NB;
	FSM_HANDLE (AI, clamp_move);
      }
    else
	fsm_queue_post_event (FSM_EVENT (AI, clamp_move_success));
}

void
clamp_move_element (uint8_t from, uint8_t to)
{
    assert (from != to);
    ctx.pos_request = from;
    ctx.moving_to = to;
    FSM_HANDLE (AI, clamp_move);
}

void
clamp_new_element (uint8_t pos, uint8_t element)
{
    assert (pos == CLAMP_SLOT_FRONT_BOTTOM || pos == CLAMP_SLOT_BACK_BOTTOM);
    ctx.pos_new = pos;
    ctx.new_element = element;
    FSM_HANDLE (AI, clamp_new_element);
}

uint8_t
clamp_handle_event (void)
{
    if (FSM_CAN_HANDLE (AI, clamp_new_element))
      {
	/* XXX: temporary hack. */
	uint8_t element = contact_get_color () ? ELEMENT_PAWN : ELEMENT_KING;
	if (!IO_GET (CONTACT_FRONT_BOTTOM)
	    && !logistic_global.slots[CLAMP_SLOT_FRONT_BOTTOM])
	  {
	    clamp_new_element (CLAMP_SLOT_FRONT_BOTTOM, element);
	    return 1;
	  }
	if (!IO_GET (CONTACT_BACK_BOTTOM)
	    && !logistic_global.slots[CLAMP_SLOT_BACK_BOTTOM])
	  {
	    clamp_new_element (CLAMP_SLOT_BACK_BOTTOM, element);
	    return 1;
	  }
      }
    return 0;
}

/** Find next position and start motors. */
static void
clamp_route (void)
{
    uint8_t pos_new;
    uint8_t pos_current = ctx.pos_current;
    uint8_t pos_request = ctx.pos_request;
    /* Compute new position. */
    if (CLAMP_IS_SLOT_IN_FRONT_BAY (pos_current))
      {
	if (!CLAMP_IS_SLOT_IN_FRONT_BAY (pos_request))
	    pos_new = CLAMP_BAY_FRONT_LEAVE;
	else
	    pos_new = pos_request;
      }
    else if (CLAMP_IS_SLOT_IN_BACK_BAY (pos_current))
      {
	if (!CLAMP_IS_SLOT_IN_BACK_BAY (pos_request))
	    pos_new = CLAMP_BAY_BACK_LEAVE;
	else
	    pos_new = pos_request;
      }
    else if (pos_current == CLAMP_SLOT_SIDE)
      {
	pos_new = CLAMP_BAY_SIDE_ENTER_LEAVE;
      }
    else if (pos_current == CLAMP_BAY_FRONT_LEAVE)
      {
	if (pos_request == CLAMP_SLOT_SIDE)
	    pos_new = CLAMP_BAY_SIDE_ENTER_LEAVE;
	else
	    pos_new = CLAMP_SLOT_BACK_MIDDLE;
      }
    else if (pos_current == CLAMP_BAY_BACK_LEAVE)
      {
	if (pos_request == CLAMP_SLOT_SIDE)
	    pos_new = CLAMP_BAY_SIDE_ENTER_LEAVE;
	else
	    pos_new = CLAMP_SLOT_FRONT_MIDDLE;
      }
    else
      {
	assert (pos_current == CLAMP_BAY_SIDE_ENTER_LEAVE);
	if (pos_request == CLAMP_SLOT_SIDE)
	    pos_new = pos_request;
	else if (CLAMP_IS_SLOT_IN_FRONT_BAY (pos_request))
	    pos_new = CLAMP_SLOT_FRONT_MIDDLE;
	else
	    pos_new = CLAMP_SLOT_BACK_MIDDLE;
      }
    /* Run motors. */
    mimot_move_motor0_absolute (clamp_pos[pos_new][0],
				BOT_CLAMP_ELEVATION_SPEED);
    mimot_move_motor1_absolute (clamp_pos[pos_new][1],
				BOT_CLAMP_ROTATION_SPEED);
    /* Remember new position. */
    ctx.pos_current = pos_new;
}

/* CLAMP FSM */

FSM_TRANS (CLAMP_START, start, CLAMP_GOING_IDLE)
{
    clamp_move (CLAMP_SLOT_FRONT_MIDDLE);
    return FSM_NEXT (CLAMP_START, start);
}

FSM_TRANS (CLAMP_GOING_IDLE, clamp_move_success, CLAMP_IDLE)
{
    return FSM_NEXT (CLAMP_GOING_IDLE, clamp_move_success);
}

FSM_TRANS (CLAMP_IDLE, clamp_new_element, CLAMP_TAKING_DOOR_CLOSING)
{
    pwm_set_timed (clamp_slot_door[ctx.pos_new], BOT_PWM_DOOR_CLOSE);
    return FSM_NEXT (CLAMP_IDLE, clamp_new_element);
}

FSM_TRANS_TIMEOUT (CLAMP_TAKING_DOOR_CLOSING, BOT_PWM_DOOR_CLOSE_TIME,
		   move_element, CLAMP_MOVING_ELEMENT,
		   move_to_idle, CLAMP_GOING_IDLE,
		   done, CLAMP_IDLE)
{
    logistic_element_new (ctx.pos_new, ctx.new_element);
    if (logistic_global.moving_from != CLAMP_SLOT_NB)
      {
	clamp_move_element (logistic_global.moving_from,
			    logistic_global.moving_to);
	return FSM_NEXT_TIMEOUT (CLAMP_TAKING_DOOR_CLOSING, move_element);
      }
    else if (logistic_global.clamp_pos_idle != ctx.pos_current)
      {
	clamp_move (logistic_global.clamp_pos_idle);
	return FSM_NEXT_TIMEOUT (CLAMP_TAKING_DOOR_CLOSING, move_to_idle);
      }
    else
	return FSM_NEXT_TIMEOUT (CLAMP_TAKING_DOOR_CLOSING, done);
}

FSM_TRANS (CLAMP_MOVING_ELEMENT, clamp_move_success,
	   move_element, CLAMP_MOVING_ELEMENT,
	   move_to_idle, CLAMP_GOING_IDLE,
	   done, CLAMP_IDLE)
{
    logistic_element_move_done ();
    if (logistic_global.moving_from != CLAMP_SLOT_NB)
      {
	clamp_move_element (logistic_global.moving_from,
			    logistic_global.moving_to);
	return FSM_NEXT (CLAMP_MOVING_ELEMENT, clamp_move_success,
			 move_element);
      }
    else if (logistic_global.clamp_pos_idle != ctx.pos_current)
      {
	clamp_move (logistic_global.clamp_pos_idle);
	return FSM_NEXT (CLAMP_MOVING_ELEMENT, clamp_move_success,
			 move_to_idle);
      }
    else
	return FSM_NEXT (CLAMP_MOVING_ELEMENT, clamp_move_success,
			 done);
}

/* CLAMP_MOVE FSM */

FSM_TRANS (CLAMP_MOVE_IDLE, clamp_move,
	   move, CLAMP_MOVE_ROUTING,
	   move_element, CLAMP_MOVE_SRC_ROUTING,
	   move_element_here, CLAMP_MOVE_SRC_CLAMP_CLOSING)
{
    if (ctx.moving_to == CLAMP_POS_NB)
      {
	clamp_route ();
	return FSM_NEXT (CLAMP_MOVE_IDLE, clamp_move, move);
      }
    else
      {
	if (ctx.pos_current != ctx.pos_request)
	  {
	    clamp_route ();
	    return FSM_NEXT (CLAMP_MOVE_IDLE, clamp_move, move_element);
	  }
	else
	  {
	    pwm_set_timed (BOT_PWM_CLAMP, BOT_PWM_CLAMP_CLOSE);
	    return FSM_NEXT (CLAMP_MOVE_IDLE, clamp_move, move_element_here);
	  }
      }
}

FSM_TRANS (CLAMP_MOVE_ROUTING, clamp_elevation_rotation_success,
	   done, CLAMP_MOVE_IDLE,
	   next, CLAMP_MOVE_ROUTING)
{
    if (ctx.pos_current == ctx.pos_request)
      {
	fsm_queue_post_event (FSM_EVENT (AI, clamp_move_success));
	return FSM_NEXT (CLAMP_MOVE_ROUTING, clamp_elevation_rotation_success,
			 done);
      }
    else
      {
	clamp_route ();
	return FSM_NEXT (CLAMP_MOVE_ROUTING, clamp_elevation_rotation_success,
			 next);
      }
}

FSM_TRANS (CLAMP_MOVE_SRC_ROUTING, clamp_elevation_rotation_success,
	   done, CLAMP_MOVE_SRC_CLAMP_CLOSING,
	   next, CLAMP_MOVE_SRC_ROUTING)
{
    if (ctx.pos_current == ctx.pos_request)
      {
	pwm_set_timed (BOT_PWM_CLAMP, BOT_PWM_CLAMP_CLOSE);
	return FSM_NEXT (CLAMP_MOVE_SRC_ROUTING,
			 clamp_elevation_rotation_success, done);
      }
    else
      {
	clamp_route ();
	return FSM_NEXT (CLAMP_MOVE_SRC_ROUTING,
			 clamp_elevation_rotation_success, next);
      }
}

FSM_TRANS_TIMEOUT (CLAMP_MOVE_SRC_CLAMP_CLOSING, BOT_PWM_CLAMP_CLOSE_TIME,
		   open_door, CLAMP_MOVE_SRC_DOOR_OPENDING,
		   move, CLAMP_MOVE_DST_ROUTING)
{
    if (clamp_slot_door[ctx.pos_current] != 0xff)
      {
	pwm_set_timed (clamp_slot_door[ctx.pos_current], BOT_PWM_DOOR_OPEN);
	return FSM_NEXT_TIMEOUT (CLAMP_MOVE_SRC_CLAMP_CLOSING, open_door);
      }
    else
      {
	ctx.pos_request = ctx.moving_to;
	clamp_route ();
	return FSM_NEXT_TIMEOUT (CLAMP_MOVE_SRC_CLAMP_CLOSING, move);
      }
}

FSM_TRANS_TIMEOUT (CLAMP_MOVE_SRC_DOOR_OPENDING, BOT_PWM_DOOR_OPEN_TIME,
		   CLAMP_MOVE_DST_ROUTING)
{
    ctx.pos_request = ctx.moving_to;
    clamp_route ();
    return FSM_NEXT_TIMEOUT (CLAMP_MOVE_SRC_DOOR_OPENDING);
}

FSM_TRANS (CLAMP_MOVE_DST_ROUTING, clamp_elevation_rotation_success,
	   done_close_door, CLAMP_MOVE_DST_DOOR_CLOSING,
	   done_open_clamp, CLAMP_MOVE_DST_CLAMP_OPENING,
	   next, CLAMP_MOVE_DST_ROUTING)
{
    if (ctx.pos_current == ctx.pos_request)
      {
	if (clamp_slot_door[ctx.pos_current] != 0xff)
	  {
	    pwm_set_timed (clamp_slot_door[ctx.pos_current],
			   BOT_PWM_DOOR_CLOSE);
	    return FSM_NEXT (CLAMP_MOVE_DST_ROUTING,
			     clamp_elevation_rotation_success,
			     done_close_door);
	  }
	else
	  {
	    pwm_set_timed (BOT_PWM_CLAMP, BOT_PWM_CLAMP_OPEN);
	    return FSM_NEXT (CLAMP_MOVE_DST_ROUTING,
			     clamp_elevation_rotation_success,
			     done_open_clamp);
	  }
      }
    else
      {
	clamp_route ();
	return FSM_NEXT (CLAMP_MOVE_DST_ROUTING,
			 clamp_elevation_rotation_success, next);
      }
}

FSM_TRANS_TIMEOUT (CLAMP_MOVE_DST_DOOR_CLOSING, BOT_PWM_DOOR_CLOSE_TIME,
		   CLAMP_MOVE_DST_CLAMP_OPENING)
{
    pwm_set_timed (BOT_PWM_CLAMP, BOT_PWM_CLAMP_OPEN);
    return FSM_NEXT_TIMEOUT (CLAMP_MOVE_DST_DOOR_CLOSING);
}

FSM_TRANS_TIMEOUT (CLAMP_MOVE_DST_CLAMP_OPENING, BOT_PWM_CLAMP_OPEN_TIME,
		   CLAMP_MOVE_IDLE)
{
    fsm_queue_post_event (FSM_EVENT (AI, clamp_move_success));
    return FSM_NEXT_TIMEOUT (CLAMP_MOVE_DST_CLAMP_OPENING);
}

