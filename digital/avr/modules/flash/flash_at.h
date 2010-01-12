#ifndef flash_at_h
#define flash_at_h
/* flash_at.h */
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
#include "flash.h"

#define FLASH_AT_PAGE_SIZE 0x200
#define FLASH_AT_BLOCK_SIZE 0x1000
#define FLASH_AT_SECTOR_SIZE 0x20000
#define FLASH_AT_SIZE 0x200000
#define FLASH_AT_ADDRESS_HIGH \
    (FLASH_AT_SIZE - 1)

#define FLASH_AT_ADDR_IS_VALID(addr) \
    ((addr) < FLASH_AT_SIZE)

#define FLASH_AT_MANUFACTURER_ID 0x1F

#define FLASH_AT_ADDR_TO_PAGE(addr) \
    ((addr) / FLASH_AT_PAGE_SIZE)

#define FLASH_AT_ADDR_TO_BLOCK(addr) \
    ((addr) / FLASH_AT_BLOCK_SIZE)

#define FLASH_AT_ADDR_TO_SECTOR(addr) \
    ((addr) / FLASH_AT_SECTOR_SIZE)

enum flash_at_erase_t
{
    FLASH_AT_ERASE_PAGE = 0x81,
    FLASH_AT_ERASE_BLOCK = 0x50,
    FLASH_AT_ERASE_SECTOR = 0x7C,
    FLASH_AT_ERASE_NB
};
typedef enum flash_at_erase_t flash_at_erase_t;

enum flash_at_cmd_t
{
    FLASH_AT_CMD_JDEC = 0x9F,
    FLASH_AT_CMD_STATUS = 0xD7,
    FLASH_AT_CMD_WRITE_BUFF1 = 0x84,
    FLASH_AT_CMD_WRITE_BUFF2 = 0x87,
    FLASH_AT_CMD_BUFF1_STORE = 0x83,
    FLASH_AT_CMD_BUFF2_STORE = 0x86,
    FLASH_AT_CMD_READ_MAIN_MEMORY = 0xD2,
    FLASH_AT_CMD_READ_BUFF1 = 0xD4,
    FLASH_AT_CMD_READ_BUFF2 = 0xD6,
    FLASH_AT_CMD_DEEP_POWER_DOWN = 0xB9,
    FLASH_AT_CMD_DEEP_POWER_DOWN_RESUME = 0xAB,
    FLASH_AT_CMD_NB
};
typedef enum flash_at_cmd_t flash_at_cmd_t;

/** Initialise the flash memory.
  */
void
flash_at_init (void);

/** Flash access.
  * The flash contains an address of 21 bits in a range from 0x0-0x1fffff.
  * This function shall access the memory directly by the SPI.
  * \param  addr  the address to provide to the flash memory.
  */
void
flash_at_address (uint32_t addr);

/** Erase the memory.
  * \param  erase_type  the erase type..
  * \param  start_addr  the start address.
  */
void
flash_at_erase (flash_erase_cmd_t cmd, uint32_t start_addr);

/* Send a flash command to the flash memory (only a command).
 * \param  cmd  the command to send.
 */
void
flash_at_send_command (flash_cmd_t cmd);

/* Send a flash command to the flash memory (only a command).
 * \param  cmd  the command to send.
 */
void
flash_at_send_command (flash_cmd_t cmd);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the status register.
  */
uint8_t
flash_at_read_status (void);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the busy bit state.
  */
uint8_t
flash_at_is_busy (void);

/** Write in the flash byte provided in parameter.
  * \param  data  the buffer to store the data.
  */
void
flash_at_write (uint32_t addr, uint8_t data);

/** Read the data at the address provided.
  * \param  addr  the address of the data to read.
  * \return  the data read.
  */
uint8_t
flash_at_read (uint32_t addr);

/** Read a data from the flash memory from the address provided and for a
 * length of the number of bytes provided.
 * \param  address at which the data should be read.
 * \param  buffer  the buffer to fill with the read data.
 * \param  length  the length of the data to read.
 */
void
flash_at_read_array (uint32_t addr, uint8_t *buffer, uint32_t length);

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to store the data.
  * \param  data  the array to store.
  * \param  length  the array length
  */
void
flash_at_write_array (uint32_t addr, uint8_t *data, uint32_t length);

/** Get the flash size.
  * \return  the flash size.
  */
uint32_t
flash_at_size (void);

/** Get the flash block size.
  * \return  the flash block size.
  */
uint32_t
flash_at_block_size (void);

#endif /* flash_at_h */
