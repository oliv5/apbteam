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

/** Define the absolute maximum PWM value. */
#define PWM_MAX 0x3ff

/** PWM control state. */
struct pwm_t
{
    /** Current PWM value. */
    int16_t cur;
    /** Maximum value. */
    int16_t max;
    /** Minimum value (dead zone). */
    int16_t min;
};

extern struct pwm_t pwm_aux[AC_ASSERV_AUX_NB];

extern uint8_t pwm_reverse;

/** Define current PWM value for each output. */
#define PWM_VALUE(x) PWM_VALUE_ (x)
#define PWM_VALUE_(x) PWM_VALUE_ ## x
#define PWM_VALUE_pwm_aux0 pwm_aux[0].cur
#define PWM_VALUE_pwm_aux1 pwm_aux[1].cur

/** Define maximum PWM value for each output. */
#define PWM_MAX_FOR(x) PWM_MAX_FOR_ (x)
#define PWM_MAX_FOR_(x) PWM_MAX_FOR_ ## x
#define PWM_MAX_FOR_pwm_aux0 PWM_MAX
#define PWM_MAX_FOR_pwm_aux1 PWM_MAX

/** Define minimum PWM value for each output, if the value is less than the
 * minimum, use 0. */
#define PWM_MIN_FOR(x) PWM_MIN_FOR_ (x)
#define PWM_MIN_FOR_(x) PWM_MIN_FOR_ ## x
#define PWM_MIN_FOR_pwm_aux0 0x10
#define PWM_MIN_FOR_pwm_aux1 0x10

/** Define which bit controls the PWM inversion. */
#define PWM_REVERSE_BIT(x) PWM_REVERSE_BIT_ (x)
#define PWM_REVERSE_BIT_(x) PWM_REVERSE_BIT_ ## x
#define PWM_REVERSE_BIT_pwm_aux0 _BV (0)
#define PWM_REVERSE_BIT_pwm_aux1 _BV (1)

/** State init macro. */
#define PWM_INIT_FOR(x) \
    { 0, PWM_MAX_FOR (x), PWM_MIN_FOR (x) }

/** Set PWM value. */
static inline void
pwm_set (struct pwm_t *pwm, int16_t value)
{
    if (value > pwm->max)
	pwm->cur = pwm->max;
    else if (value < -pwm->max)
	pwm->cur = -pwm->max;
    else if (value > -pwm->min && value < pwm->min)
	pwm->cur = 0;
    else
	pwm->cur = value;
}

void
pwm_init (void);

void
pwm_update (void);

void
pwm_set_reverse (uint8_t reverse);

#endif /* pwm_h */
