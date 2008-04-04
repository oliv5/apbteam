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
#include "modules/math/random/random.h"
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
      case c ('s', 2):
	/* Send one byte to a slave address.
	 * 2 parameters:
	 *   - slave address;
	 *   - data.
	 */
	twi_ms_send (args[0], &args[1], 1);
	while (!twi_ms_is_finished ())
	    ;
	break;
      case c ('r', 1):
	/* Read one byte from an address slave
	 * 1 parameter:
	 *   - slave address.
	 */
	  {
	    uint8_t data = 0x00;
	    int8_t d = twi_ms_read (args[0], &data, 1);
	    if (d != 0)
		proto_send0 ('e');
	    else
	      {
		while (!twi_ms_is_finished ())
		    ;
		proto_send1b ('R', data);
	      }
	  }
	break;
      case c ('r', 2):
	/* Read n bytes from an address slave
	 * 2 parameter:
	 *   - slave address.
	 *   - nb of bytes.
	 */
	  {
	    uint8_t data[16];
	    int8_t d = twi_ms_read (args[0], data, args[1]);
	    if (d != 0)
		proto_send0 ('e');
	    else
	      {
		while (!twi_ms_is_finished ())
		    ;
		proto_send ('R', args[1], data);
	      }
	  }
	break;
      case c ('t', 2):
	/* Test for sending and receiving multiple data.
	 * It sends a random number of bytes to the slave and then read back
	 * from it the same number, checking the data are the same as the sent
	 * ones.
	 * 2 parameters:
	 *   - slave address;
	 *   - number of iterations to try on.
	 */
	  {
	    int error = 0;
	    uint8_t compt = args[1];
	    while (compt-- != 0)
	      {
		/* Maximum number of bytes of the message */
		static const uint8_t max_byte_number =
		    AC_TWI_SL_RECV_BUFFER_SIZE;
		/* Get a random number */
		uint32_t random = random_u32 ();
		/* Extract a number under max_byte_number */
		uint8_t byte_number = random % max_byte_number + 1;
		/* Table which contains the random bytes */
		uint8_t random_bytes[max_byte_number];
		uint8_t i;
		for (i = 0; i < byte_number; i++)
		  {
		    uint8_t mod = i % 4;
		    /* Regenerate a new 32 bit random number every 4 bytes */
		    if ((mod == 0) && (i != 0))
			random = random_u32 ();
		    /* Get the part of the random number we need */
		    random_bytes[i] = (random >> (mod * 8)) & 0xFF;
		  }
		/* Send data */
		twi_ms_send (args[0], random_bytes, byte_number);
		while (!twi_ms_is_finished ())
		    ;
#ifdef HOST
		/* Give time to slave to copy data. */
		mex_node_wait_date (mex_node_date () + 2);
#endif

		/* Received buffer */
		uint8_t received_buffer[max_byte_number];
		/* Receive data */
		int8_t ret = twi_ms_read (args[0], received_buffer,
					  byte_number);
		/* Check return */
		if (ret != 0)
		  {
		    /* Error */
		    proto_send0 ('e');
		    error = 1;
		    break;
		  }
		while (!twi_ms_is_finished ())
		    ;
		for (i = 0; i < byte_number; i++)
		  {
		    if (received_buffer[i] != random_bytes[i])
		      {
			error = 1;
			break;
		      }
		  }
		if (error != 0)
		    break;
	      }
	    /* Check for error */
	    if (error == 0)
	      {
		proto_send1b ('T', args[1]);
	      }
	  }
	break;
      case c ('z', 0):
	/* Reset */
	utils_reset ();
	break;
      default:
	/* Variable number of arguments. */
	if (cmd == 's')
	  {
	    /* Send n bytes to a slave address.
	     * 1+n parameters:
	     *   - slave address;
	     *   - n x data.
	     */
	    twi_ms_send (args[0], &args[1], size - 1);
	    while (!twi_ms_is_finished ())
		;
	  }
	else
	  {
	    /* Error */
	    proto_send0 ('?');
	    return;
	  }
	break;
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
    twi_init (0x04);
    /* I am a master */
    proto_send0 ('M');
    while (42)
      {
#ifdef HOST
	mex_node_wait_date (mex_node_date () + 1);
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
#else
	proto_accept (uart0_getc ());
#endif
      }
    return 0;
}
