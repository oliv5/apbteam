/* test-erase.c */
/* avr.trace - AVR trace use. {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include "io.h"
#include "modules/flash/flash.h"
#include "modules/trace/trace.h"
#include "modules/proto/proto.h"
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "modules/uart/uart.h"

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    /* May be unused. */
    uint32_t addr = v8_to_v32 (0, args[0], args[1], args[2]);
    uint8_t buf[16];
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset */
	utils_reset ();
	break;
      case c ('e', 0):
	/* Erase full */
	flash_erase (FLASH_ERASE_FULL, 0);
	break;
      case c ('e', 3):
	/* Erase 4k:
	 *  - 3b: address. */
	flash_erase (FLASH_ERASE_4K, addr);
	break;
      case c ('i', 0):
	/* Initialise the trace module. */
	trace_init ();
	proto_send1b ('i', trace_status ());
	addr = trace_addr_current ();
	proto_send3b ('a', addr >> 16, addr >> 8, addr);
	break;
      case c ('u', 0):
	/* Uninitialise the trace module. */
	trace_uninit ();
	proto_send1b ('u', trace_status ());
	addr = trace_addr_current ();
	proto_send3b ('a', addr >> 16, addr >> 8, addr);
	break;
      case c ('t', 2):
	/* Trace data:
	 *  - 1b: id.
	 *  - 1b: first data. */
	TRACE (args[0], args[1]);
	break;
      case c ('r', 3):
	/* Read one byte:
	 *  - 3b: address. */
	proto_send1b ('r', flash_read (addr));
	break;
      case c ('r', 4):
	/* Read several bytes:
	 *  - 3b: address.
	 *  - 1b: number of bytes. */
	if (args[3] > sizeof (buf))
	  {
	    proto_send0 ('?');
	    return;
	  }
	else
	  {
	    flash_read_array (addr, buf, args[3]);
	    proto_send ('r', args[3], buf);
	  }
	break;
      default:
	/* Error */
	proto_send0 ('?');
	return;
      }
    /* Acknowledge what has been done */
    proto_send (cmd, size, args);
}

int
main (void)
{
    uart0_init ();
    proto_send0 ('z');
    proto_send0 ('c');

    proto_send1b ('o', sizeof (uint8_t));

    while (1)
	proto_accept (uart0_getc ());
}

