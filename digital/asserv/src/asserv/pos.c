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

/** Current theta/alpha. */
uint32_t pos_theta_cur, pos_alpha_cur;
/** Consign theta/alpha. */
uint32_t pos_theta_cons, pos_alpha_cons;

/** Error saturation. */
int32_t pos_e_sat = 1023;
/** Integral saturation. */
int32_t pos_int_sat = 1023;
/** P coefficients. */
uint16_t pos_theta_kp, pos_alpha_kp;
/** I coefficients. */
uint16_t pos_theta_ki, pos_alpha_ki;
/** D coefficients. */
uint16_t pos_theta_kd, pos_alpha_kd;
/** Blocked value.  If error is greater than this value, stop the robot and
 * report blocked state. */
int32_t pos_blocked = 15000L;

/** Current integral values. */
int32_t pos_theta_int, pos_alpha_int;
/** Last error values. */
int32_t pos_theta_e_old, pos_alpha_e_old;

/** Compute a PID.
 * How to compute maximum numbers size:
 * Result is 24 bits (16 bits kept after shift).
 * If e_sat == 1023, e max is 11 bits (do not forget the sign bit), and diff
 * max is 12 bits.
 * If int_sat == 1023, i max is 11 bits.
 * In the final addition, let's give 23 bits to the p part, and 22 bits to the
 * i and d part (23b + 22b + 22b => 23b + 23b => 24b). 
 * Therefore, kp can be 23 - 11 = 12 bits (f4.8).
 *            ki can be 22 - 11 = 11 bits (f3.8).
 *            kd can be 22 - 12 = 10 bits (f2.8).
 * How to increase this number:
 *  - lower the shift.
 *  - bound the value returned.
 *  - lower e & int saturation. */
static inline int16_t
pos_compute_pid (int32_t e, int32_t *i, int32_t *e_old,
		 uint16_t kp, uint16_t ki, uint16_t kd)
{
    int32_t diff, pid;
    /* Saturate error. */
    UTILS_BOUND (e, -pos_e_sat, pos_e_sat);
    /* Integral update. */
    *i += e;
    UTILS_BOUND (*i, -pos_int_sat, pos_int_sat);
    /* Differential value. */
    diff = e - *e_old;
    /* Compute PID. */
    pid = e * kp + *i * ki + diff * kd;
    /* Save result. */
    *e_old = e;
    return pid >> 8;
}

/** Update PWM according to consign. */
void
pos_update (void)
{
    int16_t pid_theta, pid_alpha;
    int32_t diff_theta, diff_alpha;
    /* Update current shaft positions. */
    pos_theta_cur += counter_left_diff + counter_right_diff;
    pos_alpha_cur += counter_right_diff - counter_left_diff;
    /* Compute PID. */
    diff_theta = pos_theta_cons - pos_theta_cur;
    diff_alpha = pos_alpha_cons - pos_alpha_cur;
    if (state_main.blocked
	|| diff_theta < -pos_blocked || pos_blocked < diff_theta
	|| diff_alpha < -pos_blocked || pos_blocked < diff_alpha)
      {
	/* Blocked. */
	pwm_left = 0;
	pwm_right = 0;
	state_blocked (&state_main);
      }
    else
      {
	pid_theta = pos_compute_pid (diff_theta, &pos_theta_int,
				     &pos_theta_e_old, pos_theta_kp,
				     pos_theta_ki, pos_theta_kd);
	pid_alpha = pos_compute_pid (diff_alpha, &pos_alpha_int,
				     &pos_alpha_e_old, pos_alpha_kp,
				     pos_alpha_ki, pos_alpha_kd);
	/* Update PWM. */
	pwm_left = pid_theta - pid_alpha;
	UTILS_BOUND (pwm_left, -PWM_MAX, PWM_MAX);
	pwm_right = pid_theta + pid_alpha;
	UTILS_BOUND (pwm_right, -PWM_MAX, PWM_MAX);
      }
}

/** Reset position control internal state.  To be called when the position
 * control is deactivated. */
void
pos_reset (void)
{
    pos_theta_int = pos_alpha_int = 0;
    pos_theta_cur = pos_alpha_cur = 0;
    pos_theta_cons = pos_alpha_cons = 0;
    pos_theta_e_old = pos_alpha_e_old = 0;
}

