#ifndef misc_h
#define misc_h
/* misc.h - Miscellaneous definitions. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
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

#define RED_LED(x) do { \
    if (!(x)) PORTD |= 0x20; \
    else PORTD &= ~0x20; \
} while (0)

#define GREEN_LED(x) do { \
    if (!(x)) PORTD |= 0x40; \
    else PORTD &= ~0x40; \
} while (0)

#endif /* misc_h */
