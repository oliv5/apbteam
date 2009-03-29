/* pos.c - Position motor control. */
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
#include "pos.h"

#include "modules/utils/utils.h"
#include "modules/math/fixed/fixed.h"

#include "counter.h"
#include "pwm.h"
#include "state.h"

/**
 * This file is responsible for position motor control.  The consign is a
 * position of the motor shafts, as theta/alpha.  Theta is the sum of right
 * and left position, alpha is the difference between the right and the left
 * position.
 * 16 bits are enough as long as there is no long blocking (see 2005 cup!).
 */

/** Theta/alpha control states. */
struct pos_t pos_theta, pos_alpha;

/** Auxiliaries control states. */
struct pos_t pos_aux0;

/** Error saturation. */
int32_t pos_e_sat = 1023;
/** Integral saturation. */
int32_t pos_i_sat = 1023;
/** Differential saturation. */
int32_t pos_d_sat = 1023;
/** Blocking detection: error limit. */
int32_t pos_blocked_error_limit = 2048;
/** Blocking detection: speed limit. */
int32_t pos_blocked_speed_limit = 0x10;
/** Blocking detection: counter limit. */
int32_t pos_blocked_counter_limit = 20;

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
pos_compute_pid (int32_t e, struct pos_t *pos)
{
    int32_t diff, pid;
    /* Saturate error. */
    UTILS_BOUND (e, -pos_e_sat, pos_e_sat);
    /* Integral update. */
    pos->i += e;
    UTILS_BOUND (pos->i, -pos_i_sat, pos_i_sat);
    /* Differential value. */
    diff = e - pos->e_old;
    UTILS_BOUND (diff, -pos_d_sat, pos_d_sat);
    /* Compute PID. */
    pid = e * pos->kp + pos->i * pos->ki + diff * pos->kd;
    /* Save result. */
    pos->e_old = e;
    return pid >> 8;
}

/** Update PWM according to consign. */
void
pos_update (void)
{
    if (state_main.mode >= MODE_POS)
      {
	int16_t pid_theta, pid_alpha;
	int32_t diff_theta, diff_alpha;
	/* Update current shaft positions. */
	pos_theta.cur += counter_left_diff + counter_right_diff;
	pos_alpha.cur += counter_right_diff - counter_left_diff;
	if (state_main.variant & 1)
	    pos_reset (&pos_theta);
	if (state_main.variant & 2)
	    pos_reset (&pos_alpha);
	/* Compute PID. */
	diff_theta = pos_theta.cons - pos_theta.cur;
	diff_alpha = pos_alpha.cons - pos_alpha.cur;
	/* Compute actual speed and test for blocking. */
	int32_t cur_speed_theta = pos_theta.cur - pos_theta.cur_old;
	pos_theta.cur_old = pos_theta.cur;
	int32_t cur_speed_alpha = pos_alpha.cur - pos_alpha.cur_old;
	pos_alpha.cur_old = pos_alpha.cur;
	if ((UTILS_ABS (diff_theta) > pos_blocked_error_limit
	     && UTILS_ABS (cur_speed_theta) < pos_blocked_speed_limit))
	    pos_theta.blocked_counter++;
	else
	    pos_theta.blocked_counter = 0;
	if ((UTILS_ABS (diff_alpha) > pos_blocked_error_limit
	     && UTILS_ABS (cur_speed_alpha) < pos_blocked_speed_limit))
	    pos_alpha.blocked_counter++;
	else
	    pos_alpha.blocked_counter = 0;
	if (pos_theta.blocked_counter > pos_blocked_counter_limit
	    || pos_alpha.blocked_counter > pos_blocked_counter_limit)
	  {
	    /* Blocked. */
	    pos_reset (&pos_theta);
	    pos_reset (&pos_alpha);
	    state_blocked (&state_main);
	    pwm_set (&pwm_left, 0);
	    pwm_set (&pwm_right, 0);
	  }
	else
	  {
	    pid_theta = pos_compute_pid (diff_theta, &pos_theta);
	    pid_alpha = pos_compute_pid (diff_alpha, &pos_alpha);
	    /* Update PWM. */
	    pwm_set (&pwm_left, pid_theta - pid_alpha);
	    pwm_set (&pwm_right, pid_theta + pid_alpha);
	  }
      }
    if (state_aux0.mode >= MODE_POS)
      {
	int16_t pid;
	int32_t diff;
	/* Update current shaft position. */
	pos_aux0.cur += counter_aux0_diff;
	/* Compute PID. */
	diff = pos_aux0.cons - pos_aux0.cur;
	if (UTILS_ABS (diff) > 5000)
	  {
	    /* Blocked. */
	    pos_reset (&pos_aux0);
	    state_blocked (&state_aux0);
	    pwm_set (&pwm_aux0, 0);
	  }
	else
	  {
	    pid = pos_compute_pid (diff, &pos_aux0);
	    /* Update PWM. */
	    pwm_set (&pwm_aux0, pid);
	  }
      }
}

/** Reset position control state.  To be called when the position control is
 * deactivated. */
void
pos_reset (struct pos_t *pos)
{
    pos->i = 0;
    pos->cur = 0;
    pos->cons = 0;
    pos->e_old = 0;
    pos->cur_old = 0;
    pos->blocked_counter = 0;
}

