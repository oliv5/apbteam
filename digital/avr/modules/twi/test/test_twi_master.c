/* test_twi_master.c */
/* avr.twi.master - TWI master module. {{{
 *
 * Copyright (C) 2005 Clément Demonchy
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
#include "modules/proto/proto.h"
#include "modules/twi/twi.h"
#include "modules/utils/utils.h"
#include "io.h"

void proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    uint8_t data[1] = {0x00};
    int8_t c;
    switch (cmd)
      {
      case 's':
	if (size == 2)
	  {
	    proto_send ('s', 0, 0);
	    twi_ms_send (args[0], &args[1], 1);
	    while (!twi_ms_is_finished ())
		;
	    proto_send ('f', 0, 0);
	  }
	else
	    proto_send ('e', 0, 0);
	break;
      case 'r':
	if (size == 2)
	  {
	    uart0_putc ('r');
	    c = twi_ms_read (args[0], data, 1);
	    if (c != 0)
		proto_send ('e', 1 , 0);
	    else
	      {
		while (!twi_ms_is_finished ())
		    ;
		proto_send ('f', 1, data);
	      }
	  }
	else
	    proto_send ('e', 1, 0);
	break;
      case 'z':
	utils_reset ();
	break;
      default:
	proto_send ('e', 1, 0);
      }
}

int
main (void)
{
    uint8_t c;
    sei ();
    uart0_init ();
    twi_init (0x04);
    uart0_putc ('m');
    uart0_putc ('s');
    uart0_putc ('s');
    uart0_putc ('\r');
    while (42)
      {
	c = uart0_getc ();
	proto_accept (c);
      }
    return 0;
}
