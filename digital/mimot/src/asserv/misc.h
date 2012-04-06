#ifndef misc_h
#define misc_h
/* misc.h - Miscellaneous definitions. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
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

#define MISC_SETUP do { \
    /* Pull-ups. */ \
    PORTB = 0xe0; \
    PORTC = 0xfc; \
    PORTD = 0x80; \
} while (0)

#define LED_SETUP do { \
} while (0)

#define LED1(x) do { \
} while (0)

#define LED2(x) do { \
} while (0)

#endif /* misc_h */
