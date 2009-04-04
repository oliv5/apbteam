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
struct speed_t speed_aux[AC_ASSERV_AUX_NB];

/** Initialise speed control states. */
void
speed_init (void)
{
    uint8_t i;
    speed_theta.pos = &pos_theta;
    speed_alpha.pos = &pos_alpha;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	speed_aux[i].pos = &pos_aux[i];
}

/** Update current speed according to a speed consign. */
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
     * The "+ 0xff" is to ceil result.
     * s = sqrt (2 * a * d) */
    s = fixed_sqrt_ui32 ((2 * UTILS_ABS (d) * acc + 0xff) >> 8);
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

/** Update current speed according to a position consign. */
static void
speed_update_by_position (struct speed_t *speed)
{
    int32_t diff = speed->pos_cons - speed->pos->cons;
    speed->cur = speed_compute_max_speed (diff, speed->cur, speed->acc,
					  speed->max);
}

/** Update shaft position consign according to its consign type. */
static void
speed_update_by (struct speed_t *speed)
{
    if (speed->use_pos)
	speed_update_by_position (speed);
    else
	speed_update_by_speed (speed);
    /* Update shaft position. */
    speed->pos->cons += speed->cur >> 8;
}

/** Update shaft position consign for two motors system. */
static void
speed_update_double (struct state_t *state, struct speed_t *speed0,
		     struct speed_t *speed1)
{
    if (state->mode >= MODE_SPEED)
      {
	/* Update speed. */
	speed_update_by (speed0);
	speed_update_by (speed1);
	/* Check for completion. */
	if (state->mode == MODE_SPEED
	    && (speed0->use_pos || speed1->use_pos)
	    && (!speed0->use_pos || speed0->cur == 0)
	    && (!speed1->use_pos || speed1->cur == 0))
	  {
	    state_finish (state);
	  }
      }
}

/** Update shaft position consign for one motor system. */
static void
speed_update_single (struct state_t *state, struct speed_t *speed)
{
    if (state->mode >= MODE_SPEED)
      {
	/* Update speed. */
	speed_update_by (speed);
	/* Check for completion. */
	if (state->mode == MODE_SPEED
	    && speed->use_pos && speed->cur == 0)
	    state_finish (state);
      }
}

/** Update shaft position consign according to consign. */
void
speed_update (void)
{
    uint8_t i;
    speed_update_double (&state_main, &speed_theta, &speed_alpha);
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	speed_update_single (&state_aux[i], &speed_aux[i]);
}

