/* test_twi.c */
/* avr.twi - TWI AVR module. {{{
 *
 * Copyright (C) 2005 Clément Demonchy
 * Copyright (C) 2010 Nicolas Schodet
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
      case c ('z', 0):
	/* Reset. */
	utils_reset ();
	break;
#if AC_TWI_MASTER_ENABLE
      case c ('r', 2):
	/* Read n bytes from a slave.
	 * - b: slave address.
	 * - b: nb of bytes. */
	  {
	    uint8_t data[args[1]];
	    twi_master_recv (args[0], data, args[1]);
	    uint8_t r = twi_master_wait ();
	    proto_send ('R', r, data);
	  }
	break;
      case c ('t', 3):
	/* Test for sending and receiving multiple data.
	 * It sends a random number of bytes to the slave and then read back
	 * from it the same number, checking the data are the same as the sent
	 * ones.
	 * - slave address.
	 * - maximum nb of bytes.
	 * - number of iterations to try on.
	 */
	  {
	    int error = 0;
	    uint8_t max_byte_number = args[1];
	    uint8_t compt = args[2];
	    while (compt-- != 0 && !error)
	      {
		/* Maximum number of bytes of the message */
		/* Get a random number */
		uint32_t random = random_u32 ();
		/* Extract a number under max_byte_number */
		uint8_t byte_number = random % max_byte_number + 1;
		/* Table which contains the random bytes */
		uint8_t random_bytes[max_byte_number];
		uint8_t i, ret;
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
		twi_master_send (args[0], random_bytes, byte_number);
		ret = twi_master_wait ();
		/* Check return */
		if (ret != byte_number)
		  {
		    /* Error */
		    proto_send3b ('S', compt, ret, byte_number);
		    error = 1;
		    break;
		  }
#ifdef HOST
		/* Give time to slave to copy data. */
		mex_node_wait_date (mex_node_date () + 2);
#endif
		/* Received buffer */
		uint8_t received_buffer[max_byte_number];
		/* Receive data */
		twi_master_recv (args[0], received_buffer, byte_number);
		ret = twi_master_wait ();
		/* Check return */
		if (ret != byte_number)
		  {
		    /* Error */
		    proto_send3b ('R', compt, ret, byte_number);
		    error = 1;
		    break;
		  }
		for (i = 0; i < byte_number; i++)
		  {
		    if (received_buffer[i] != random_bytes[i])
		      {
			/* Error */
			proto_send3b ('C', compt, i, byte_number);
			error = 1;
			break;
		      }
		  }
	      }
	  }
	break;
#endif /* AC_TWI_MASTER_ENABLE */
      default:
	/* Variable number of arguments. */
#if AC_TWI_MASTER_ENABLE
	if (cmd == 's')
	  {
	    /* Send n bytes to a slave.
	     * - b: slave address.
	     * - xb: x data. */
	    twi_master_send (args[0], &args[1], size - 1);
	    uint8_t r = twi_master_wait ();
	    proto_send1b ('S', r);
	  }
	else
#endif /* AC_TWI_MASTER_ENABLE */
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
    /* Enable interruptions. */
    sei ();
    /* Initialize serial port. */
    uart0_init ();
    /* We have successfully boot. */
    proto_send0 ('z');
    /* Initialize TWI. */
    twi_init (0x04);
#if AC_TWI_MASTER_ENABLE
    /* I am a master. */
    proto_send0 ('M');
#endif
#if AC_TWI_SLAVE_ENABLE
    /* I am a slave. */
    proto_send0 ('S');
#endif
    while (42)
      {
#ifdef HOST
	mex_node_wait_date (mex_node_date () + 1);
#endif
#if AC_TWI_SLAVE_ENABLE
	uint8_t data[AC_TWI_SLAVE_RECV_BUFFER_SIZE];
	uint8_t data_len;
	/* Check for data. */
	data_len = twi_slave_poll (data, AC_TWI_SLAVE_RECV_BUFFER_SIZE);
	if (data_len)
	  {
	    /* Echo them back.*/
	    twi_slave_update (data, data_len);
	  }
#endif
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
      }
}

