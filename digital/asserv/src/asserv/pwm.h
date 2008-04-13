#ifndef pwm_h
#define pwm_h
/* pwm.h */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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

/** Define the absolute maximum PWM value.
 * This value is lowered until the bug relatives to maximum value is fixed
 * (rounding after shifting bug). */
#define PWM_MAX 0x3f0

extern int16_t pwm_left, pwm_right, pwm_aux0;
extern uint8_t pwm_reverse;

#define PWM_REVERSE_BIT(x) PWM_REVERSE_BIT_ (x)
#define PWM_REVERSE_BIT_(x) PWM_REVERSE_BIT_ ## x
#define PWM_REVERSE_BIT_pwm_left _BV (0)
#define PWM_REVERSE_BIT_pwm_right _BV (1)
#define PWM_REVERSE_BIT_pwm_aux0 _BV (2)

void
pwm_init (void);

void
pwm_update (void);

void
pwm_set_reverse (uint8_t reverse);

#endif /* pwm_h */
