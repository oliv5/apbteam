/* pwm.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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

#include "modules/utils/utils.h"
#include "io.h"

/** Delay for LMD18200. */
#ifndef HOST
# define PWM_LMD18200_DELAY_US 1
#else
# define PWM_LMD18200_DELAY_US 0
#endif

#ifdef HOST
extern uint16_t OCR1A;
extern uint8_t PORTB;
#endif

/** Stop timer. PWM is set to zero when it reaches zero. */
uint16_t pwm_stop_timer;

/** Initialise PWM. */
void
pwm_init (void)
{
#ifndef HOST
    /* Fast PWM, TOP = 0x3ff, OCnA, OCnB & OCnC with positive logic.
       f_IO without prescaler.
       Fpwm = f_IO / (prescaler * (1 + TOP)) = 14400 Hz. */
    TCCR1A =
	regv (COM1A1, COM1A0, COM1B1, COM1B0, COM1C1, COM1C0, WGM11, WGM10,
		   1,      0,      1,      0,      1,      0,     1,     1);
    TCCR1B = regv (ICNC1, ICES1, 5, WGM13, WGM12, CS12, CS11, CS10,
		       0,     0, 0,     0,     1,    0,    0,    1);
    /* Enable pwm and direction outputs in DDRB. */
    IO_DDR (PWM_IO) |= IO_BV (PWM_IO);
    IO_DDR (PWM_DIR_IO) |= IO_BV (PWM_DIR_IO);
#endif
}

/**
 * Set a PWM for a given time.
 *  - value: PWM value (-PWM_MAX..+PWM_MAX).
 *  - timer: timer after which the PWM is stopped, or 0 for infinite.
 */
void
pwm_set (int16_t value, uint16_t timer)
{
    if (value == 0)
      {
	PWM_OCR = 0;
      }
    else
      {
	/* Ensure value is not over PWM_MAX. */
	UTILS_BOUND (value, -PWM_MAX, PWM_MAX);
	if (value > 0)
	  {
	    IO_PORT (PWM_DIR_IO) |= IO_BV (PWM_DIR_IO);
	    if (PWM_LMD18200_DELAY_US)
		utils_delay_us (PWM_LMD18200_DELAY_US);
	    PWM_OCR = value;
	  }
	else
	  {
	    IO_PORT (PWM_DIR_IO) &= ~IO_BV (PWM_DIR_IO);
	    if (PWM_LMD18200_DELAY_US)
		utils_delay_us (PWM_LMD18200_DELAY_US);
	    PWM_OCR = -value;
	  }
      }
    pwm_stop_timer = timer;
}

/** Update PWM, should be called at regular interval. */
void
pwm_update (void)
{
    if (pwm_stop_timer && --pwm_stop_timer == 0)
	pwm_set (0, 0);
}

