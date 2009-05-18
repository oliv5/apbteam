/* flash.host.c */
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

void
flash_address (uint32_t addr){}

void
flash_erase (uint8_t cmd, uint32_t start_addr){}

void
flash_send_command (uint8_t cmd){}

uint8_t
flash_read_status (void)
{
    return 0;
}

/** Initialise the flash memory.
  * \return true if the flash is present, false otherwise.
  */
uint8_t
flash_init (void)
{
    return 0;
}

void
flash_write (uint32_t addr, uint8_t data) {}

uint8_t
flash_read (uint32_t addr) { return 0xff; }

void
flash_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
}

void
flash_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{}

int8_t
flash_log (uint8_t size, uint8_t *args)
{
    return 1;
}
