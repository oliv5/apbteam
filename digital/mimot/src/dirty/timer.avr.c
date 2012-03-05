/* timer.avr.c */
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

#include "modules/utils/utils.h"
#include "modules/motor/encoder/encoder.h"
#include "io.h"

/** Top timer value. */
#define TIMER_TOP 255
/** Number of steps during wait. */
#define TIMER_STEPS 4
/** Size of step. */
#define TIMER_STEP ((TIMER_TOP + 1) / TIMER_STEPS)

/** Initialise the timer. */
void
timer_init (void)
{
    TCCR0 = regv (FOC0, WGM00, COM01, COM00, WGM01, CS02, CS01, CS00,
		     0,     0,     0,     0,     0,    1,    0,    0);
    /* Fov = F_io / (prescaler * (TOP + 1))
     * TOP = 0xff
     * prescaler = 256
     * Tov = 1 / Fov = 4.444 ms */
}

/** Wait for timer overflow. */
void
timer_wait (void)
{
    uint8_t i;
    /* Make small steps with counter updates. */
    for (i = 1; i < TIMER_STEPS; i++)
      {
	while (TCNT0 < i * TIMER_STEP)
	    ;
	encoder_update_step ();
      }
    /* Wait overflow. */
    while (!(TIFR & _BV (TOV0)))
	;
    /* Write 1 to clear. */
    TIFR = _BV (TOV0);
}

