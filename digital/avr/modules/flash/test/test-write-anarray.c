/* test-write-anarray.c */
/* avr.flash - AVR Flash SPI use. {{{
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
#include "../flash.h"
#include "modules/proto/proto.h"
#include "modules/utils/utils.h"
#include "modules/uart/uart.h"

uint32_t addr = 0;

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
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
      case c ('s', 0):
	/* print flash status */
	proto_send1b ('s', flash_read_status());
      case c ('w', 0):
	/* Send the write enable flash command. */
	flash_send_command (FLASH_WREN);
      case c ('a', 3):
	/* get the addr */
	addr = ((uint32_t) (args[0] & 0x1F) << 16) | ((uint32_t) args[1] << 8)
	| args[2];
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
    uint8_t data[5];
    uint8_t data_rsp[5];
    uint8_t i;

    uart0_init ();
    proto_send0 ('z');
    proto_send0 ('c');
    flash_init ();
    proto_send0 ('f');

    proto_send1b ('s', flash_read_status ());
    
    for (i = 0; i < 5; i++)
      {
	data[i] = i + 'a';
      }
    
    /* Write a full array. */
    flash_write_array (addr, data, 5);

    proto_send1b ('p', flash_read_status());

    /* Read a full array. */
    flash_read_array (addr , data_rsp, 5);

    /* Print the data_rsp to the i2c */
    proto_send ('g', 5, data_rsp);

    addr += 5;

    while (1)
	proto_accept (uart0_getc ());
}

