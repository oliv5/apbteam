/* pawn_sensor.c */
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
#include "defs.h"
#include "pawn_sensor.h"

#include "asserv.h"
#include "contact.h"
#include "logistic.h"
#include "element.h"
#include "clamp.h"
#include "bot.h"
#include "playground.h"
#include "codebar.h"
#include "mimot.h"
#include "main.h"

#define FSM_NAME AI
#include "fsm.h"
#include "fsm_queue.h"

#include "modules/utils/utils.h"
#include "modules/math/geometry/distance.h"

/* Handle pawn sensors.  When a pawn is detected, it can not be taken
 * directly, but only once it is inside the robot. */

/** Pawn sensor context. */
struct pawn_sensor_t
{
    /** Is there something in front of the sensor? */
    uint8_t active;
    /** If active, supposed position of element. */
    vect_t active_position;
};

/** Pawn sensor general context. */
struct pawn_sensor_general_t
{
    /** Activate bumpers. */
    uint8_t bumper_enabled;
    /** Last bumped element position. */
    vect_t last_bumped;
    /** Bumper triggered, wait until the next one. */
    uint16_t bump_wait;
};

/** Global contexts. */
struct pawn_sensor_t pawn_sensor_front, pawn_sensor_back;
struct pawn_sensor_general_t pawn_sensor_global;

static uint8_t
pawn_sensor_get_type (uint8_t direction)
{
    uint8_t element_type = codebar_get (direction);
    if (!element_type)
	element_type = ELEMENT_PAWN;
    return element_type;
}

uint8_t
pawn_sensor_get (uint8_t direction)
{
    struct pawn_sensor_t *ctx;
    uint8_t contact_value, slot;
    int16_t dist;
    /* Check direction. */
    if (direction == DIRECTION_FORWARD)
      {
	ctx = &pawn_sensor_front;
	contact_value = !IO_GET (CONTACT_FRONT_BOTTOM);
	slot = CLAMP_SLOT_FRONT_BOTTOM;
	dist = BOT_PAWN_FRONT_DETECTION_THRESHOLD_MM;
      }
    else
      {
	ctx = &pawn_sensor_back;
	contact_value = !IO_GET (CONTACT_BACK_BOTTOM);
	slot = CLAMP_SLOT_BACK_BOTTOM;
	dist = BOT_PAWN_BACK_DETECTION_THRESHOLD_MM;
      }
    /* Handle contact. */
    if (contact_value)
      {
	if (!logistic_global.slots[slot]
	    && logistic_global.moving_to != slot)
	  {
	    position_t robot_position;
	    asserv_get_position (&robot_position);
	    if (ctx->active)
	      {
		/* In green zone, take it when near enougth from the wall. */
		if (robot_position.v.x < BOT_GREEN_ELEMENT_DISTANCE_MM + 10
		    || (robot_position.v.x > PG_WIDTH
			- BOT_GREEN_ELEMENT_DISTANCE_MM - 10))
		  {
		    ctx->active = 0;
		    return pawn_sensor_get_type (direction);
		  }
		/* Else, take it if near enough from the supposed element
		 * position. */
		int32_t d = distance_point_point (&ctx->active_position,
						  &robot_position.v);
		if (d < BOT_PAWN_TAKING_DISTANCE_MM)
		  {
		    ctx->active = 0;
		    return pawn_sensor_get_type (direction);
		  }
		else if (d > UTILS_ABS (dist))
		  {
		    vect_from_polar_uf016 (&ctx->active_position, dist,
					   robot_position.a);
		    vect_translate (&ctx->active_position, &robot_position.v);
		  }
	      }
	    else
	      {
		ctx->active = 1;
		vect_from_polar_uf016 (&ctx->active_position, dist,
				       robot_position.a);
		vect_translate (&ctx->active_position, &robot_position.v);
	      }
	  }
      }
    else
      {
	ctx->active = 0;
      }
    return 0;
}

void
pawn_sensor_bumper (uint8_t bumped, uint16_t dx, uint16_t dy)
{
    uint8_t i;
    if (bumped)
      {
	/* Compute pawn position. */
	position_t robot_pos;
	asserv_get_position (&robot_pos);
	vect_t bumped_pawn;
	bumped_pawn.x = dx;
	bumped_pawn.y = dy;
	vect_rotate_uf016 (&bumped_pawn, robot_pos.a);
	vect_translate (&bumped_pawn, &robot_pos.v);
	/* Do not count if out of the table. */
	if (bumped_pawn.x < 400 + BOT_ELEMENT_RADIUS
	    && bumped_pawn.x >= PG_WIDTH - 400 - BOT_ELEMENT_RADIUS
	    && bumped_pawn.y < BOT_ELEMENT_RADIUS
	    && bumped_pawn.y >= PG_WIDTH - BOT_ELEMENT_RADIUS)
	    return;
	/* Do not count the opponent as a pawn. */
	for (i = 0; i < main_obstacles_nb; i++)
	  {
	    uint16_t dist = distance_point_point (&bumped_pawn,
						  &main_obstacles_pos[i]);
	    if (dist < 300 + BOT_ELEMENT_RADIUS)
		return;
	  }
	/* OK, take it. */
	pawn_sensor_global.last_bumped = bumped_pawn;
	fsm_queue_post_event (FSM_EVENT (AI, top_bumper));
	pawn_sensor_global.bump_wait = 3 * 250;
      }
}

void
pawn_sensor_update (void)
{
#define BUMPER_FRONT_LEFT _BV (6)
#define BUMPER_FRONT_RIGHT _BV (7)
#define BUMPER_BACK_RIGHT _BV (5)
#define BUMPER_BACK_LEFT _BV (4)
    if (pawn_sensor_global.bumper_enabled)
      {
	if (pawn_sensor_global.bump_wait)
	    pawn_sensor_global.bump_wait--;
	else
	  {
	    uint8_t bumpers = mimot_get_input ();
	    pawn_sensor_bumper (!(bumpers & BUMPER_FRONT_LEFT), 120, 265);
	    pawn_sensor_bumper (!(bumpers & BUMPER_FRONT_RIGHT), 120, -265);
	    pawn_sensor_bumper (!(bumpers & BUMPER_BACK_RIGHT), -120, -265);
	    pawn_sensor_bumper (!(bumpers & BUMPER_BACK_LEFT), -120, 265);
	  }
      }
}

void
pawn_sensor_bumper_enable (uint8_t enabled)
{
    pawn_sensor_global.bumper_enabled = enabled;
}

vect_t
pawn_sensor_get_last_bumped (void)
{
    return pawn_sensor_global.last_bumped;
}

