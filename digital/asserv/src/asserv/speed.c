/* speed.c - Speed control. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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

/** Current speed, f8.8. */
int16_t speed_theta_cur, speed_alpha_cur;
/** Consign speed, f8.8. */
int16_t speed_theta_cons, speed_alpha_cons;
/** Maximum speed for position consign, u8. */
int8_t speed_theta_max, speed_alpha_max;
/** Slow speed for position consign, u8. */
int8_t speed_theta_slow, speed_alpha_slow;
/** Consign position. */
uint32_t speed_theta_pos_cons, speed_alpha_pos_cons;
/** Whether to use the consign position (1) or not (0). */
uint8_t speed_pos;

/** Acceleration, uf8.8. */
int16_t speed_theta_acc, speed_alpha_acc;

/** Update shaft position consign according to a speed consign. */
static void
speed_update_by_speed (void)
{
    /* Update current speed. */
    if (UTILS_ABS (speed_theta_cons - speed_theta_cur) < speed_theta_acc)
	speed_theta_cur = speed_theta_cons;
    else if (speed_theta_cons > speed_theta_cur)
	speed_theta_cur += speed_theta_acc;
    else
	speed_theta_cur -= speed_theta_acc;
    if (UTILS_ABS (speed_alpha_cons - speed_alpha_cur) < speed_alpha_acc)
	speed_alpha_cur = speed_alpha_cons;
    else if (speed_alpha_cons > speed_alpha_cur)
	speed_alpha_cur += speed_alpha_acc;
    else
	speed_alpha_cur -= speed_alpha_acc;
}

/** Compute maximum allowed speed according to: distance left, maximum speed,
 * current speed and acceleration. */
static int16_t
speed_compute_max_speed (int32_t d, int16_t cur, int16_t acc, int8_t max)
{
    int16_t s;
    /* Compute maximum speed in order to be able to brake in time.
     * s = sqrt (2 * a * d) */
    s = fixed_sqrt_ui32 (2 * (acc >> 8) * UTILS_ABS (d));
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
speed_update_by_position (void)
{
    int32_t theta_d = speed_theta_pos_cons - pos_theta_cons;
    int32_t alpha_d = speed_alpha_pos_cons - pos_alpha_cons;
    if (theta_d >= -speed_theta_max && theta_d <= speed_theta_max)
	speed_theta_cur = theta_d << 8;
    else
	speed_theta_cur = speed_compute_max_speed (theta_d, speed_theta_cur,
		speed_theta_acc, speed_theta_max);
    if (alpha_d >= -speed_alpha_max && alpha_d <= speed_alpha_max)
	speed_alpha_cur = alpha_d << 8;
    else
	speed_alpha_cur = speed_compute_max_speed (alpha_d, speed_alpha_cur,
		speed_alpha_acc, speed_alpha_max);
    if (speed_theta_cur == 0 && speed_alpha_cur == 0)
	state_finish (&state_main);
}

/** Update shaft position consign according to consign. */
void
speed_update (void)
{
    /* Update speed. */
    if (speed_pos)
	speed_update_by_position ();
    else
	speed_update_by_speed ();
    /* Update shaft position. */
    pos_theta_cons += speed_theta_cur >> 8;
    pos_alpha_cons += speed_alpha_cur >> 8;
}

