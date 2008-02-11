/* timer.avr.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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

/* +AutoDec */

#include "modules/utils/utils.avr.h"

/** Initialise the timer. */
static inline void
timer_init (void);

/** Wait for timer overflow. */
static inline void
timer_wait (void);

/** Read timer value. Used for performance analysis. */
static inline uint8_t
timer_read (void);

/* -AutoDec */

/** Initialise the timer. */
static inline void
timer_init (void)
{
    TCCR0 = regv (FOC0, WGM00, COM01, COM0, WGM01, CS02, CS01, CS00,
		     0,     0,     0,    0,     0,    1,    1,    0);
    /* Fov = F_io / (prescaler * (TOP + 1))
     * TOP = 0xff
     * prescaler = 256
     * Tov = 1 / Fov = 4.444 ms */
}

/** Wait for timer overflow. */
static inline void
timer_wait (void)
{
    while (!(TIFR & _BV (TOV0)))
	;
    /* Write 1 to clear. */
    TIFR = _BV (TOV0);
}

/** Read timer value. Used for performance analysis. */
static inline uint8_t
timer_read (void)
{
    return TCNT0;
}

