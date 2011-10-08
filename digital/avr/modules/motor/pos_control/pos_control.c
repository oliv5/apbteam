/* pos_control.c */
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
#include "pos_control.h"

#include "modules/utils/utils.h"

/** Compute a PID.
 * How to compute maximum numbers size:
 * Result is 24 bits (16 bits kept after shift).
 * If e_sat == 1023, e max is 11 bits (do not forget the sign bit), and diff
 * max is 12 bits (can be saturated with d_sat).
 * If i_sat == 1023, i max is 11 bits.
 * In the final addition, let's give 23 bits to the p part, and 22 bits to the
 * i and d part (23b + 22b + 22b => 23b + 23b => 24b).
 * Therefore, kp can be 23 - 11 = 12 bits (f4.8).
 *            ki can be 22 - 11 = 11 bits (f3.8).
 *            kd can be 22 - 12 = 10 bits (f2.8).
 * How to increase this number:
 *  - lower the shift.
 *  - bound the value returned.
 *  - lower e, i & d saturation. */
static inline int16_t
pos_control_compute_pid (struct pos_control_t *pos_control, int32_t e)
{
    int32_t diff, pid;
    /* Saturate error. */
    UTILS_BOUND (e, -pos_control->e_sat, pos_control->e_sat);
    /* Integral update. */
    pos_control->i += e;
    UTILS_BOUND (pos_control->i, -pos_control->i_sat, pos_control->i_sat);
    /* Differential value. */
    diff = e - pos_control->last_error;
    UTILS_BOUND (diff, -pos_control->d_sat, pos_control->d_sat);
    /* Compute PID. */
    pid = e * pos_control->kp + pos_control->i * pos_control->ki
	+ diff * pos_control->kd;
    /* Save result. */
    pos_control->last_error = e;
    return pid >> 8;
}

/** Reset position control so that it does not go crazy when enabled. */
static void
pos_control_reset (pos_control_t *pos_control)
{
    pos_control->cons = pos_control->cur;
    pos_control->i = 0;
    pos_control->last_error = 0;
}

void
pos_control_init (pos_control_t *pos_control)
{
    pos_control_reset (pos_control);
}

int16_t
pos_control_update (pos_control_t *pos_control, int16_t cur_speed,
		    uint8_t enabled)
{
    int32_t error;
    int16_t pid;
    /* Update current shaft position. */
    pos_control->cur += cur_speed;
    /* Control position if enabled. */
    if (enabled)
      {
	/* Compute error. */
	error = pos_control->cons - pos_control->cur;
	/* Compute PID. */
	pid = pos_control_compute_pid (pos_control, error);
      }
    else
      {
	pos_control_reset (pos_control);
	pid = 0;
      }
    return pid;
}

