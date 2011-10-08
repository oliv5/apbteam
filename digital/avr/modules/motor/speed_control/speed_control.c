/* speed_control.c */
/* motor - Motor control module. {{{
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
#include "speed_control.h"

#include "modules/utils/utils.h"
#include "modules/math/fixed/fixed.h"

/** Update current speed according to a speed consign. */
static void
speed_control_update_by_speed (speed_control_t *speed_control)
{
    /* Update current speed (be careful of overflow!). */
    if (speed_control->cons > speed_control->cur)
      {
	if ((uint16_t) (speed_control->cons - speed_control->cur)
	    < (uint16_t) speed_control->acc)
	    speed_control->cur = speed_control->cons;
	else
	    speed_control->cur += speed_control->acc;
      }
    else
      {
	if ((uint16_t) (speed_control->cur - speed_control->cons)
	    < (uint16_t) speed_control->acc)
	    speed_control->cur = speed_control->cons;
	else
	    speed_control->cur -= speed_control->acc;
      }
}

/** Compute maximum allowed speed according to: distance left, maximum speed,
 * current speed and acceleration. */
static int16_t
speed_control_compute_max_speed (int32_t d, int16_t cur, int16_t acc,
				 int8_t max)
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
speed_control_update_by_position (speed_control_t *speed_control,
				  pos_control_t *pos_control)
{
    int32_t diff = speed_control->pos_cons - pos_control->cons;
    speed_control->cur = speed_control_compute_max_speed
	(diff, speed_control->cur, speed_control->acc, speed_control->max);
}

void
speed_control_init (speed_control_t *speed_control,
		    pos_control_t *pos_control)
{
    speed_control->pos_control = pos_control;
}

void
speed_control_update (speed_control_t *speed_control,
		      uint8_t enabled)
{
    if (enabled)
      {
	/* Compute speed. */
	if (speed_control->use_pos)
	    speed_control_update_by_position (speed_control,
					      speed_control->pos_control);
	else
	    speed_control_update_by_speed (speed_control);
	/* Update shaft position. */
	speed_control->pos_control->cons += speed_control->cur >> 8;
      }
    else
	speed_control->cur = 0;
}

void
speed_control_set_speed (speed_control_t *speed_control, int8_t speed)
{
    speed_control->use_pos = 0;
    speed_control->cons = speed << 8;
}

void
speed_control_pos_offset (speed_control_t *speed_control, int32_t offset)
{
    speed_control->use_pos = 1;
    speed_control->pos_cons = speed_control->pos_control->cons + offset;
}

void
speed_control_pos_offset_from_here (speed_control_t *speed_control,
				    int32_t offset)
{
    speed_control->use_pos = 1;
    speed_control->pos_cons = speed_control->pos_control->cur + offset;
}

void
speed_control_pos (speed_control_t *speed_control, int32_t pos_cons)
{
    speed_control->use_pos = 1;
    speed_control->pos_cons = pos_cons;
}

void
speed_control_hard_stop (speed_control_t *speed_control)
{
    /* No future movement. */
    speed_control->use_pos = 0;
    speed_control->cur = 0;
    speed_control->cons = 0;
    /* Really stop right here, position control on the current point. */
    speed_control->pos_control->cons = speed_control->pos_control->cur;
}

