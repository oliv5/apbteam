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
    TCCR0 = regv (FOC0, WGM00, COM01, COM0, WGM01, CS02, CS01, CS00,
                     0,     0,     0,    0,     0,    1,    1,    0);
}

uint8_t
timer_wait (void)
{
    /* Let's pretend we have reached overflow before calling this function. */
    uint8_t count_before_ov = 1;
    /* Loop until an overflow of the timer occurs. */
    while (!(TIFR & _BV (TOV0)))
	/* We have not reached overflow. */
	count_before_ov = 0;
    /* Write 1 to clear overflow. */
    TIFR = _BV (TOV0);
    return count_before_ov;
}

