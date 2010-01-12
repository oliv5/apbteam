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

#define FLASH_LOG_PAGE_SIZE 0x80000
#define FLASH_LOG_BUFFER_SIZE 16

/** Enable CS. */
#define FLASH_CS_ENABLE \
    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS)

/** Disable CS. */
#define FLASH_CS_DISABLE \
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);

enum flash_cmd_t
{
    FLASH_CMD_INIT,
    FLASH_CMD_READ_BYTE,
    FLASH_CMD_WRITE_BYTE,
    FLASH_CMD_READ,
    FLASH_CMD_WRITE,
    FLASH_CMD_NB
};
typedef enum flash_cmd_t flash_cmd_t;

enum flash_erase_cmd_t
{
    FLASH_ERASE_PAGE,
    FLASH_ERASE_SECTOR,
    FLASH_ERASE_BLOCK,
    FLASH_ERASE_FULL,
    FLASH_ERASE_NB
};
typedef enum flash_erase_cmd_t flash_erase_cmd_t;

/** Initialise the flash memory.
  * \return true if the flash is present, false otherwise.
  */
uint8_t
flash_init (void);

/** Erase the memory.
  * \param  erase_type  the erase type..
  * \param  start_addr  the start address.
  */
void
flash_erase (flash_erase_cmd_t cmd, uint32_t start_addr);

/* Send a flash command to the flash memory (only a command).
 * \param  cmd  the command to send.
 */
void
flash_send_command (flash_cmd_t cmd);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the status register.
  */
uint8_t
flash_read_status (void);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the busy bit state.
  */
uint8_t
flash_is_busy (void);

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
  * \return - 0 on success,
  *         - 1 flash not initialised
  *         - 2 flash read error.
  *         - 3 command not found.
  */
int8_t
flash_log (uint8_t size, uint8_t *args);

/** Get the flash size.
  * \return  the flash size.
  */
uint32_t
flash_size (void);

/** Get the flash block size.
  * \return  the flash block size.
  */
uint32_t
flash_block_size (void);

/** Initialise all the function pointer for SST flash memory.
  */
void
flash_init_sst (void);

/** Initialise all the function pointer for ATMEL flash memory.
  */
void
flash_init_at (void);

#endif /* flash_h */
