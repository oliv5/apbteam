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

#define FLASH_ADDRESS_HIGH 0x1FFFFF
#define FLASH_ADDRESS_ERROR 0xFFFFFF
#define FLASH_ADDRESS_INC(val) \
    ((val) + 1) & FLASH_ADDRESS_HIGH

#define FLASH_PAGE_SIZE  0x1000
#define FLASH_PAGE_MASK (FLASH_ADDRESS_HIGH & ~(FLASH_PAGE_SIZE-1))
#define FLASH_PAGE(val) ((val) & FLASH_PAGE_MASK)

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
#define FLASH_AAI 0xAD

#define FLASH_TBP_US 10

enum
{
    FLASH_CMD_INIT,
    FLASH_CMD_READ,
    FLASH_CMD_NB
};

/** Flash access.
  * The flash contains an address of 21 bits in a range from 0x0-0x1fffff.
  * This function shall access the memory directly by the SPI.
  * \param  addr  the address to provide to the flash memory.
  */
void
flash_address (uint32_t addr);

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

/** Poll the busy bit in the Software Status Register of the flash memory.
  * \return  the status register.
  */
uint8_t
flash_read_status (void);

/** Poll the busy bit in the Software Status Register of the flash memory.
  * \return  the busy bit state.
  */
extern inline uint8_t
flash_is_busy (void)
{
    return flash_read_status () & 0x1;
}

/** Return the AAI status flag of the register.
  * \return  the AAI status.
  */
extern inline uint8_t
flash_status_aai (void)
{
    return flash_read_status () >> 6;
}

/** Initialise the flash memory.
  * \return true if the flash is present, false otherwise.
  */
uint8_t
flash_init (void);

/** Find the first writable sector.
 * \param  addr  the address to start the research.
 * \return  the address of the next sector.
 */
uint32_t
flash_first_sector (void);

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
 * \param  address at which the data should be read.
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

/** Process the logs
  * \param  size  the number of arguments.
  * \param  an array of arguments.
  * \return  true on success.
  */
uint8_t
flash_log (uint8_t size, uint8_t *args);

#endif /* flash_h */
