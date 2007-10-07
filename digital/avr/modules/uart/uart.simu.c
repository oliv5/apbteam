/* uart.simu.c - used for simulavrxx pseudo ports. */
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
#include "common.h"
#include "uart.h"

#include "uart_common.h"

#if AC_UART (PORT) != -1

int uart (eof);

/** Initialise uart. */
void
uart_init (void)
{
    uart (eof) = 0;
}

/** Use this as exit point. */
void
uart (exit) (void)
{
    uart (eof) = 1;
}

/** Read a char. */
uint8_t
uart_getc (void)
{
    uint8_t c;
    if (uart (eof))
	return 0xff;
    c = *(uint8_t *) 0x20;
    if (!c)
      {
	uart (exit) ();
	c = 0xff;
      }
    return c;
}

/** Write a char. */
void
uart_putc (uint8_t c)
{
    *(uint8_t *) 0x22 = c;
}

/** Retrieve availlable chars. */
uint8_t
uart_poll (void)
{
    return !uart (eof);
}

#endif /* AC_UART (PORT) != -1 */
