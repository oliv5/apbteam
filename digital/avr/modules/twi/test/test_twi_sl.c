/* test_twi_slave.c */
/* avr.twi.slave - TWI slave module. {{{
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

#ifdef HOST
# include "modules/host/mex.h"
#endif

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
	/* Reset */
      case 'z':
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
main (int argc, char **argv)
{
    avr_init (argc, argv);
#ifdef HOST
    mex_node_connect ();
#endif
    /* Enable interruptions */
    sei ();
    /* Initialize serial port */
    uart0_init ();
    /* We have successfully boot */
    proto_send0 ('z');
    /* Initialize TWI */
    twi_init (0x02);
    /* I am a slave */
    proto_send0 ('S');
    while (42)
      {
#ifdef HOST
	mex_node_wait_date (mex_node_date () + 1);
#endif
	uint8_t data[AC_TWI_SL_RECV_BUFFER_SIZE];
	data[0] = 0;
	/* Check for data */
	if (twi_sl_poll (data, AC_TWI_SL_RECV_BUFFER_SIZE))
	  {
	    /* Receive and store them */
	    twi_sl_update (data, AC_TWI_SL_RECV_BUFFER_SIZE);
	  }
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
      }
    return 0;
}
