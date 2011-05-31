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
#include "pawn_sensor.h"

#include "asserv.h"
#include "contact.h"
#include "logistic.h"
#include "element.h"
#include "clamp.h"
#include "bot.h"

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

/** Global contexts. */
struct pawn_sensor_t pawn_sensor_front, pawn_sensor_back;

static uint8_t
pawn_sensor_get_type (uint8_t direction)
{
    uint8_t element_type = IO_GET (CONTACT_STRAT) ? ELEMENT_PAWN : ELEMENT_KING;
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

