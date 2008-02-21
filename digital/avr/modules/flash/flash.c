/* flash.c */
/*  {{{
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
#include "flash.h"
#include "spi.h"

static flash_t flash_global; 

/** Initialise the flsah memory.
  */
void
flash_init (void)
{
    flash_global.addr = 0x0;
}

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to store the data.
  * \param  data  the buffer to store the data.
  */
void
flash_write (uint8_t addr, uint8_t data)
{
    spi_init (SPI_IT_DISABLE | SPI_ENABLE | SPI_MSB_FIRST | SPI_MASTER |
	      SPI_CPOL_RISING | SPI_CPHA_SAMPLE | SPI_FOSC_DIV2); 
    spi_send (addr);
    spi_send (data);
}

/** Read the data at the address provided.
  * \param  addr  the address of the data to read.
  * \return  the data read.
  */
uint8_t
flash_read (uint8_t addr)
{
    spi_init (SPI_IT_DISABLE | SPI_ENABLE | SPI_MSB_FIRST | SPI_MASTER |
	      SPI_CPOL_RISING | SPI_CPHA_SAMPLE | SPI_FOSC_DIV2); 
    spi_send (addr);
    return spi_recv (data);
}

/** Read a data from the flash memory from the address provided and for a
 * length of the number of bytes provided.
 * \param  address at wich the data should be read.
 * \param  buffer  the buffer to fill with the read data.
 * \param  length  the length of the data to read.
 */
void
flash_read_array (uint8_t addr, uint8_t *buffer, uint8_t length);

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to store the data.
  * \param  data  the array to store.
  * \param  length  the array length
  */
void
flash_write_array (uint8_t addr, uint8_t *data, uint8_t length);

/** Read the memory from the address automaticaly managed, the offset of the
 * data shall be provided to get the data.
 * \param offset  the offset from the current address managed.
 * \return data  read from the memory.
 */
uint8_t
flash_read_managed (uint8_t offset);

/** Read an array of data from the memory starting at the address less the
 * offset provided in parameters. The data read will be stored in the buffer
 * provided in memory too. The buffer shall have at list the length of the
 * offset provided.
 * \param  offset  the offset of the current position to read the data.
 * \param  buffer  the buffer to store the data.
 */
void
flash_read_managed_array (uint8_t offset, uint8_t *buffer);

/** Write a data with a managed array.
  * \param  data to store in the memory.
  */
flash_write_managed (uint8_t data);

/** Write an array of data to the flash memory. The length of the array shall
 * be provided.
 * \param  buffer  the buffer containing the data to write.
 * \param  length  the data length to write.
 */
void
flash_write_managed_array (uint8_t *buffer, uint8_t length);

