/* flash_at.host.c */
/* avr.flash - AVR Flash SPI use. {{{
 *
 * Copyright (C) 2009 Nelio Laranjeiro
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
#include "flash_at.h"
#include "flash_io.host.h"

void
flash_at_init (void)
{
    flash_io_init (FLASH_AT_SIZE);
}

void
flash_at_address (uint32_t addr)
{ }

void
flash_at_erase (flash_erase_cmd_t cmd, uint32_t start_addr)
{
    if (FLASH_AT_ADDR_IS_VALID (start_addr))
      {
	uint32_t length;
	switch (cmd)
	  {
	  case FLASH_ERASE_PAGE:
	    length = FLASH_AT_PAGE_SIZE;
	    break;
	  case FLASH_ERASE_SECTOR:
	    length = FLASH_AT_SECTOR_SIZE;
	    break;
	  case FLASH_ERASE_BLOCK:
	    length = FLASH_AT_BLOCK_SIZE;
	    break;
	  case FLASH_ERASE_FULL:
	    length = FLASH_AT_SIZE;
	  default:
	    return;
	  }
	flash_io_erase (start_addr, length);
      }
}

void
flash_at_send_command (flash_cmd_t cmd)
{ }

uint8_t
flash_at_read_status (void)
{
    return 0;
}

uint8_t
flash_at_is_busy (void)
{
    /* Not busy. */
    return 0;
}

void
flash_at_write (uint32_t addr, uint8_t data)
{
    if (FLASH_AT_ADDR_IS_VALID (addr))
	flash_io_write (addr, data);
}

uint8_t
flash_at_read (uint32_t addr)
{
    if (FLASH_AT_ADDR_IS_VALID (addr))
	return flash_io_read (addr);
    return 0xff;
}

void
flash_at_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    if (FLASH_AT_ADDR_IS_VALID (addr))
	flash_io_read_array (addr, buffer, length);
}

void
flash_at_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    if (FLASH_AT_ADDR_IS_VALID (addr))
	flash_io_write_array (addr, data, length);
}
