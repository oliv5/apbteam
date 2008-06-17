#ifndef uart_h
#define uart_h
/* uart.h */
/* avr.uart - UART AVR module. {{{
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

/** Initialise uart. */
void
uart0_init (void);

/** Read a char. */
uint8_t
uart0_getc (void);

/** Write a char. */
void
uart0_putc (uint8_t c);

/** Retrieve available chars. */
uint8_t
uart0_poll (void);

/** Initialise uart. */
void
uart1_init (void);

/** Read a char. */
uint8_t
uart1_getc (void);

/** Write a char. */
void
uart1_putc (uint8_t c);

/** Retrieve available chars. */
uint8_t
uart1_poll (void);

#endif /* uart_h */
