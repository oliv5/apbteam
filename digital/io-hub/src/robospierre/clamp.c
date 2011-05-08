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

#define FSM_NAME AI
#include "fsm.h"

#include "mimot.h"
#include "bot.h"

FSM_INIT

FSM_STATES (
	    /* Wait order. */
	    CLAMP_IDLE,
	    /* Moving to a final or intermediary position. */
	    CLAMP_ROUTING)

FSM_EVENTS (
	    /* Order to move the clamp. */
	    clamp_move,
	    /* Elevation and elevation motor success. */
	    clamp_elevation_rotation_success,
	    /* Elevation motor failure. */
	    clamp_elevation_failure,
	    /* Rotation motor failure. */
	    clamp_rotation_failure)

FSM_START_WITH (CLAMP_IDLE)

/** Clamp context. */
struct clamp_t
{
    /* Current position. */
    uint8_t pos_current;
    /* Requested position. */
    uint8_t pos_request;
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

void
clamp_move (uint8_t pos)
{
    if (pos != ctx.pos_current)
      {
	ctx.pos_request = pos;
	FSM_HANDLE (AI, clamp_move);
      }
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

FSM_TRANS (CLAMP_IDLE, clamp_move, CLAMP_ROUTING)
{
    clamp_route ();
    return FSM_NEXT (CLAMP_IDLE, clamp_move);
}

FSM_TRANS (CLAMP_ROUTING, clamp_elevation_rotation_success,
	   done, CLAMP_IDLE,
	   next, CLAMP_ROUTING)
{
    if (ctx.pos_current == ctx.pos_request)
      {
	return FSM_NEXT (CLAMP_ROUTING, clamp_elevation_rotation_success,
			 done);
      }
    else
      {
	clamp_route ();
	return FSM_NEXT (CLAMP_ROUTING, clamp_elevation_rotation_success,
			 next);
      }
}

