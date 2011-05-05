#ifndef pwm_h
#define pwm_h
/* pwm.h */
/* io-hub - Modular Input/Output. {{{
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

/** Handle PWM output with direction.  Also handle timed PWM setting to apply
 * a PWM value for a given number of update, then switch to a resting value.
 *
 * AC_IOHUB_PWM should be set to a list of space separated PWM macro call:
 *
 * PWM (timer, pwm, pwm_io_port, pwm_io_n, dir_io_port, dir_io_n)
 *
 * For example:
 *
 * PWM (1, A, B, 1, B, 2) => use output compare A on timer 1, PWM output on
 * B1, direction on B2. */

/** Maximum positive value. */
#define PWM_MAX 0x3ff

/** Initialise PWM module. */
void
pwm_init (void);

/** Update PWM module (handle timed PWM). */
void
pwm_update (void);

/** Set PWM value for given PWM index. */
void
pwm_set (uint8_t index, int16_t value);

/** Set PWM value for given PWM index for a given time.  After time elapsed,
 * use a rest value. */
void
pwm_set_timed (uint8_t index, int16_t value, uint16_t time, int16_t value_rest);

#endif /* pwm_h */
