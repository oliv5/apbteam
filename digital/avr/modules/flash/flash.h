#ifndef flash_h
#define flash_h
/* flash.h */
/* avr.modules - AVR modules. {{{
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

#define FLASH_HIGH_ADDRESS 0x1FFFFF
#define FLASH_ADDRESS_INC_MASK(val) (val = (val+1) & FLASH_HIGH_ADDRESS)

#define FLASH_PAGE_SIZE  0x1000
#define FLASH_PAGE_MASK (0xFFF)
#define FLASH_PAGE(val) (val & FLASH_PAGE_MASK)

#define FLASH_ERASE_FULL 0x60
#define FLASH_ERASE_4K 0x20
#define FLASH_ERASE_32K 0x52
#define FLASH_ERASE_64K 0xD8

#define FLASH_READ_ID 0x9F
#define FLASH_READ 0x03
#define FLASH_WRITE 0x2
#define FLASH_WRSR 0x1
#define FLASH_RDSR 0x5
#define FLASH_WREN 0x6
#define FLASH_WEDI 0x4
#define FLASH_EWSR 0x50

struct flash_t
{
    /** Current Address in the flash memory. */
    uint32_t addr;
};
typedef struct flash_t flash_t;

/** Initialise the flsah memory.
  */
void
flash_init (void);

/** Flash access.
  * The flash contains an address of 21 bits in a range from 0x0-0x1fffff.
  * This function shall access the memory directly by the SPI.
  * \param  addr  the address to provide to the flash memory.
  */
void
flash_address (uint32_t addr);

/** Flash init page.
  * Initialise the page by write an value different of 0xFF to indicate that
  * the page is use.
  * \param addr
  */
void
flash_init_page (uint32_t addr);

/** Erase the memory.
  * \param  erase_type  the erase type..
  * \param  start_addr  the start address.
  */
void
flash_erase (uint8_t cmd, uint32_t start_addr);

/* Send a flash command to the flash memory (only a command).
 * \param  cmd  the command to send.
 */
void
flash_send_command (uint8_t cmd);

/** Write in the flash byte provided in parameter.
  * \param  data  the buffer to store the data.
  */
void
flash_write (uint32_t addr, uint8_t data);

/** Read the data at the address provided.
  * \param  addr  the address of the data to read.
  * \return  the data read.
  */
uint8_t
flash_read (uint32_t addr);

/** Read a data from the flash memory from the address provided and for a
 * length of the number of bytes provided.
 * \param  address at wich the data should be read.
 * \param  buffer  the buffer to fill with the read data.
 * \param  length  the length of the data to read.
 */
void
flash_read_array (uint32_t addr, uint8_t *buffer, uint32_t length);

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to store the data.
  * \param  data  the array to store.
  * \param  length  the array length
  */
void
flash_write_array (uint32_t addr, uint8_t *data, uint32_t length);

/** Read the memory from the address automaticaly managed, the offset of the
 * data shall be provided to get the data.
 * \param offset  the offset from the current address managed.
 * \return data  read from the memory.
 */
uint8_t
flash_read_managed (uint32_t offset);

/** Read an array of data from the memory starting at the address less the
 * offset provided in parameters. The data read will be stored in the buffer
 * provided in memory too. The buffer shall have at list the length of the
 * offset provided.
 * \param  offset  the offset of the current position to read the data.
 * \param  buffer  the buffer to store the data.
 */
void
flash_read_managed_array (uint32_t offset, uint8_t *buffer);

/** Write a data with a managed array.
  * \param  data to store in the memory.
  */
void
flash_write_managed (uint8_t data);

/** Write an array of data to the flash memory. The length of the array shall
 * be provided.
 * \param  buffer  the buffer containing the data to write.
 * \param  length  the data length to write.
 */
void
flash_write_managed_array (uint8_t *buffer, uint8_t length);

#endif /* flash_h */
