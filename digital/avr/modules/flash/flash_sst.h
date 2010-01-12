#ifndef flash_sst_h
#define flash_sst_h
/* flash_sst.h */
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
#define FLASH_SST_SIZE 0x200000
#define FLASH_SST_BLOCK_SIZE 0x10000
#define FLASH_SST_ADDRESS_HIGH (FLASH_SST_SIZE - 1)

#define FLASH_SST_ADDR_IS_VALID(addr) \
    ((addr) < FLASH_SST_SIZE)

#define FLASH_SST_TBP_US 10
#define FLASH_SST_MANUFACTURER_ID 0xBF

enum flash_sst_erase_t
{
    FLASH_SST_ERASE_FULL = 0x60,
    FLASH_SST_ERASE_4K = 0x20,
    FLASH_SST_ERASE_32K = 0x52,
    FLASH_SST_ERASE_64K = 0xD8,
    FLASH_SST_ERASE_NB
};
typedef enum flash_sst_erase_t flash_sst_erase_t;

enum flash_sst_cmd_t
{
    FLASH_SST_CMD_READ_ID = 0x9F,
    FLASH_SST_CMD_READ = 0x03,
    FLASH_SST_CMD_WRITE = 0x2,
    FLASH_SST_CMD_WRSR = 0x1,
    FLASH_SST_CMD_RDSR = 0x5,
    FLASH_SST_CMD_WREN = 0x6,
    FLASH_SST_CMD_WEDI = 0x4,
    FLASH_SST_CMD_EWSR = 0x50,
    FLASH_SST_CMD_AAI = 0xAD,
    FLASH_SST_CMD_NB
};
typedef enum flash_sst_cmd_t flash_sst_cmd_t;

/** Initialise the flash memory.
  */
void
flash_sst_init (void);

/** Erase the memory.
  * \param  erase_type  the erase type..
  * \param  start_addr  the start address.
  */
void
flash_sst_erase (flash_erase_cmd_t cmd, uint32_t start_addr);

/* Send a flash command to the flash memory (only a command).
 * \param  cmd  the command to send.
 */
void
flash_sst_send_command (flash_cmd_t cmd);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the status register.
  */
uint8_t
flash_sst_read_status (void);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the busy bit state.
  */
uint8_t
flash_sst_is_busy (void);

/** Write in the flash byte provided in parameter.
  * \param  data  the buffer to store the data.
  */
void
flash_sst_write (uint32_t addr, uint8_t data);

/** Read the data at the address provided.
  * \param  addr  the address of the data to read.
  * \return  the data read.
  */
uint8_t
flash_sst_read (uint32_t addr);

/** Read a data from the flash memory from the address provided and for a
 * length of the number of bytes provided.
 * \param  address at which the data should be read.
 * \param  buffer  the buffer to fill with the read data.
 * \param  length  the length of the data to read.
 */
void
flash_sst_read_array (uint32_t addr, uint8_t *buffer, uint32_t length);

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to store the data.
  * \param  data  the array to store.
  * \param  length  the array length
  */
void
flash_sst_write_array (uint32_t addr, uint8_t *data, uint32_t length);

/** Get the flash size.
  * \return  the flash size.
  */
uint32_t
flash_sst_size (void);

/** Get the flash block size.
  * \return  the flash block size.
  */
uint32_t
flash_sst_block_size (void);

#endif /* flash_sst_h */
