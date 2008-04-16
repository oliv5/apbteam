/* speed.c - Speed control. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
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
#include "speed.h"

#include "modules/utils/utils.h"
#include "modules/math/fixed/fixed.h"

#include "pos.h"
#include "state.h"

/**
 * This file is responsible for speed control.  It changes the current shafts
 * positions using ramps.  It can be controlled by a wanted speed or wanted
 * shaft position.
 */

/** Theta/alpha speed control states. */
struct speed_t speed_theta, speed_alpha;

/** Auxiliaries speed control states. */
struct speed_t speed_aux0;

/** Update shaft position consign according to a speed consign. */
static void
speed_update_by_speed (struct speed_t *speed)
{
    /* Update current speed. */
    if (UTILS_ABS (speed->cons - speed->cur) < speed->acc)
	speed->cur = speed->cons;
    else if (speed->cons > speed->cur)
	speed->cur += speed->acc;
    else
	speed->cur -= speed->acc;
}

/** Compute maximum allowed speed according to: distance left, maximum speed,
 * current speed and acceleration. */
static int16_t
speed_compute_max_speed (int32_t d, int16_t cur, int16_t acc, int8_t max)
{
    int16_t s;
    /* Compute maximum speed in order to be able to brake in time.
     * s = sqrt (2 * a * d) */
    s = fixed_sqrt_ui32 ((2 * UTILS_ABS (d) * acc) >> 8);
    /* Apply consign. */
    s = UTILS_MIN (max, s);
    /* Apply sign. */
    if (d < 0)
	s = -s;
    /* Convert to f8.8 and check acceleration. */
    s = s << 8;
    UTILS_BOUND (s, cur - acc, cur + acc);
    return s;
}

/** Update shaft position consign according to a position consign. */
static void
speed_update_by_position (struct speed_t *speed, struct pos_t *pos)
{
    int32_t diff = speed->pos_cons - pos->cons;
    if (diff >= -speed->max && diff <= speed->max)
	speed->cur = diff << 8;
    else
	speed->cur = speed_compute_max_speed (diff, speed->cur, speed->acc,
					      speed->max);
}

/** Update shaft position consign according to its consign type. */
static void
speed_update_by (struct speed_t *speed, struct pos_t *pos)
{
    if (speed->use_pos)
	speed_update_by_position (speed, pos);
    else
	speed_update_by_speed (speed);
    /* Update shaft position. */
    pos->cons += speed->cur >> 8;
}

/** Update shaft position consign according to consign. */
void
speed_update (void)
{
    if (state_main.mode >= MODE_SPEED)
      {
	/* Update speed. */
	speed_update_by (&speed_theta, &pos_theta);
	speed_update_by (&speed_alpha, &pos_alpha);
	/* Check for completion. */
	if ((speed_theta.use_pos || speed_alpha.use_pos)
	    && (!speed_theta.use_pos || speed_theta.cur == 0)
	    && (!speed_alpha.use_pos || speed_alpha.cur == 0))
	  {
	    state_finish (&state_main);
	  }
      }
    if (state_aux0.mode >= MODE_SPEED)
      {
	/* Update speed. */
	speed_update_by (&speed_aux0, &pos_aux0);
	/* Check for completion. */
	if (speed_aux0.use_pos && speed_aux0.cur == 0)
	    state_finish (&state_aux0);
      }
}

