/* pwm.avr.c - Handle all PWM generators. */
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
#include "pwm.h"
#include "pwm_mp.avr.h"
#include "pwm_ocr.avr.h"

/** PWM control states. */
struct pwm_t pwm_left = PWM_INIT_FOR (pwm_left);
struct pwm_t pwm_right = PWM_INIT_FOR (pwm_right);
struct pwm_t pwm_aux0 = PWM_INIT_FOR (pwm_aux0);
/** PWM reverse directions. */
uint8_t pwm_reverse;

/** Initialise PWM generators. */
void
pwm_init (void)
{
    pwm_mp_init ();
    pwm_ocr_init ();
}

/** Update the hardware PWM values. */
void
pwm_update (void)
{
    pwm_mp_update ();
    pwm_ocr_update ();
}

/** Set which PWM is reversed. */
void
pwm_set_reverse (uint8_t reverse)
{
    pwm_reverse = reverse;
    pwm_ocr_set_reverse (reverse);
}

