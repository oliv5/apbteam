#ifndef flash_io_host_h
#define flash_io_host_h
/* flash_io.host.h */
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
#include "common.h"

/** Erase the memory.
  * \param  start_addr  the start address.
  * \param  size  the size to erase.
  */
void
flash_io_erase (uint32_t start_addr, uint32_t size);

/** Initialise the flash memory.
  * \param  flash_size  the flash size.
  * \return true if the flash is present, false otherwise.
  */
uint8_t
flash_io_init (uint32_t flash_size);

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to write the data.
  * \param  data  the buffer to store the data.
  */
void
flash_io_write (uint32_t addr, uint8_t data);

/** Read the data at the address provided.
  * \param  addr  the address of the data to read.
  * \return  the data read.
  */
uint8_t
flash_io_read (uint32_t addr);

/** Read a data from the flash memory from the address provided and for a
 * length of the number of bytes provided.
 * \param  addr  at which the data should be read.
 * \param  buffer  the buffer to fill with the read data.
 * \param  length  the length of the data to read.
 */
void
flash_io_read_array (uint32_t addr, uint8_t *buffer, uint32_t length);

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to store the data.
  * \param  data  the array to store.
  * \param  length  the array length
  */
void
flash_io_write_array (uint32_t addr, uint8_t *data, uint32_t length);

#endif /* flash_io_host_h */
