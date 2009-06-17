/* flash.c */
/* avr.flash - AVR Flash SPI use. {{{
 *
 * Copyright (C) 2009 Nélio Laranjeiro
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
#include "flash.h"
#include "modules/proto/proto.h"

int8_t
flash_log (uint8_t size, uint8_t *args)
{
    uint8_t buf[FLASH_LOG_BUFFER_SIZE+1];
    int8_t error = 0x0;
    uint32_t addr = 0;

    if (size >= 4)
	addr = (((uint32_t) args[1]) << 16)
	    | (((uint32_t) args[2]) << 8) | args[3];

    switch (args[0])
      {
      case FLASH_CMD_INIT:
	error = !flash_init ();
	proto_send1b ('s', error ? 0 : 1);
	break;
      case FLASH_CMD_READ:
	if ((size == 5)
	    && (args[4] <= sizeof(buf)))
	  {
	    flash_read_array (addr, buf, args[4]);
	    proto_send ('r', args[4], buf);
	    error = 0;
	  }
	else if (size == 4)
	  {
	    proto_send1b ('r', flash_read (addr));
	    error = 0;
	  }
	else
	    error = 2;
	break;
      default:
	return 3;
      }

    return error;
}
