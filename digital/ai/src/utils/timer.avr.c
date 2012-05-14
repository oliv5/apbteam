/* timer.avr.c */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2009 Dufour Jérémy
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

#include "timer.h"

#include "modules/utils/utils.h"
#include "io.h"

/** Set to 1 when timer overflowed, reset in timer_wait. */
static volatile uint8_t timer_overflow;

/** Incremented when timer overflowed. */
static volatile uint8_t timer_tick;

ISR (TIMER0_OVF_vect)
{
    timer_overflow = 1;
    timer_tick++;
}

void
timer_init (void)
{
    /* Configuration of the timer/counter 0:
     *  - top = 0xff,
     *  - prescaler = 256,
     *  -> Fov = F_io / (prescaler * (TOP + 1))
     *  -> Tov = 1 / Fov
     * Note: if you change the TCCR0 register value, please also update
     * TIMER_TC0_PRESCALER and TIMER_TC0_TOP. */
#ifdef TCCR0
# define TIFR_reg TIFR
    TCCR0 = regv (FOC0, WGM00, COM01, COM0, WGM01, CS02, CS01, CS00,
                     0,     0,     0,    0,     0,    1,    1,    0);
#else
# define TIFR_reg TIFR0
    TCCR0A = regv (COM0A1, COM0A0, COM0B1, COM0B0, 3, 2, WGM01, WGM00,
		        0,      0,      0,      0, 0, 0,     0,     0);
    TCCR0B = regv (FOC0A, FOC0B, 5, 4, WGM02, CS02, CS01, CS00,
		       0,     0, 0, 0,     0,    1,    0,    0);
#endif
#ifdef TIMSK0
    TIMSK0 = _BV (TOIE0);
#else
    TIMSK |= _BV (TOIE0);
#endif
}

uint8_t
timer_wait (void)
{
    uint8_t late = 1;
    while (!timer_overflow)
	late = 0;
    timer_overflow = 0;
    return late;
}

uint8_t
timer_get_tick (void)
{
    return timer_tick;
}

