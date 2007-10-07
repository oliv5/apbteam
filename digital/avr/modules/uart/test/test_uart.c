/* test_uart.c */
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
#include "modules/uart/uart.h"
#include "modules/utils/utils.h"
#include "io.h"

int
main (int argc, char **argv)
{
    avr_init (argc, argv);
    sei ();
    uart0_init ();
    uart0_putc ('N');
    uart0_putc ('i');
    uart0_putc ('0');
    uart0_putc ('!');
    uart0_putc ('\r');
#if AC_UART1_PORT != -1
    uart1_init ();
    uart1_putc ('N');
    uart1_putc ('i');
    uart1_putc ('1');
    uart1_putc ('!');
    uart1_putc ('\r');
    while (1)
      {
	utils_delay (0.1);
	if (uart0_poll ())
	  {
	    char c = uart0_getc ();
	    if (c == 'z')
		utils_reset ();
	    uart0_putc (c);
	  }
	if (uart1_poll ())
	    uart1_putc (uart1_getc ());
      }
#else
    while (1)
      {
	char c = uart0_getc ();
	if (c == 'z')
	    utils_reset ();
	uart0_putc (c);
      }
#endif
}
