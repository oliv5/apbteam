/* test_spi.c */
/* avr.spi - SPI AVR module. {{{
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
#include "modules/proto/proto.h"
#include "modules/uart/uart.h"
#include "modules/spi/spi.h"
#include "modules/utils/utils.h"

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    /* TODO: command to select a slave. */
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	utils_reset ();
	break;
      case c ('s', 1):
	spi_send (args[0]);
	break;
      case c ('r', 0):
	proto_send1b ('R', spi_recv ());
	break;
      case c ('r', 1):
	proto_send1b ('R', spi_send_and_recv (args[0]));
	break;
      default:
	proto_send0 ('?');
	return;
      }
    proto_send (cmd, size, args);
#undef c
}

int
main (void)
{
    spi_init (SPI_MASTER, SPI_MODE_0, SPI_MSB_FIRST, SPI_FOSC_DIV128);
    uart0_init ();
    sei ();
    proto_send0 ('z');
    while (1)
	proto_accept (uart0_getc ());
}

