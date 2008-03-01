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
#include "modules/twi/twi.h"
#include "modules/proto/proto.h"
#include "modules/uart/uart.h"
#include "modules/utils/utils.h"
#include "io.h"

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
	/* Send one byte to a slave address */
      case c ('s', 2):
	/* s destination_addr data */
	twi_ms_send (args[0], &args[1], 1);
	while (!twi_ms_is_finished ())
	    ;
	break;
	/* Read one byte from an address slave */
      case c ('r', 1):
	/* c slave_address */
	  {
	    uint8_t data[1] = {0x00};
	    int8_t d = twi_ms_read (args[0], data, 1);
	    if (d != 0)
		proto_send0 ('e');
	    else
	      {
		while (!twi_ms_is_finished ())
		    ;
		proto_send ('R', 1, data);
	      }
	  }
	break;
	/* Reset */
      case c ('z', 0):
	utils_reset ();
	break;
	/* Error */
      default:
	proto_send0 ('?');
	return;
      }
    /* Acknowledge what has been done */
    proto_send (cmd, size, args);
}

int
main (void)
{
    /* Enable interruptions */
    sei ();
    /* Initialize serial port */
    uart0_init ();
    /* We have successfully boot */
    proto_send0 ('z');
    /* Initialize TWI */
    twi_init (0x04);
    /* I am a master */
    proto_send0 ('M');
    while (42)
      {
	uint8_t c = uart0_getc ();
	proto_accept (c);
      }
    return 0;
}
