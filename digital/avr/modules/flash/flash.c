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
#include "flash_sst.h"
#include "flash_at.h"
#include "modules/proto/proto.h"

/** Erase the memory.
  * \param  erase_type  the erase type..
  * \param  start_addr  the start address.
  */
typedef void
(*flash_erase_t) (flash_erase_cmd_t cmd, uint32_t start_addr);

/* Send a flash command to the flash memory (only a command).
 * \param  cmd  the command to send.
 */
typedef void
(*flash_send_command_t) (flash_cmd_t cmd);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the status register.
  */
typedef uint8_t
(*flash_read_status_t) (void);

/** Read the busy bit in the Software Status Register of the flash memory.
  * \return  the busy bit state.
  */
typedef uint8_t
(*flash_is_busy_t) (void);

/** Write in the flash byte provided in parameter.
  * \param  data  the buffer to store the data.
  */
typedef void
(*flash_write_t) (uint32_t addr, uint8_t data);

/** Read the data at the address provided.
  * \param  addr  the address of the data to read.
  * \return  the data read.
  */
typedef uint8_t
(*flash_read_t) (uint32_t addr);

/** Read a data from the flash memory from the address provided and for a
 * length of the number of bytes provided.
 * \param  address at which the data should be read.
 * \param  buffer  the buffer to fill with the read data.
 * \param  length  the length of the data to read.
 */
typedef void
(*flash_read_array_t) (uint32_t addr, uint8_t *buffer, uint32_t length);

/** Write in the flash byte provided in parameter.
  * \param  addr  the address to store the data.
  * \param  data  the array to store.
  * \param  length  the array length
  */
typedef void
(*flash_write_array_t) (uint32_t addr, uint8_t *data, uint32_t length);

/** Get the flash size.
  * \return  the flash size.
  */
typedef uint32_t
(*flash_size_t) (void);

/** Get the flash block size.
  * \return  the flash block size.
  */
typedef uint32_t
(*flash_block_size_t) (void);

struct flash_t
{
    /** Erase function. */
    flash_erase_t erase_func;
    /** Send command function. */
    flash_send_command_t send_cmd_func;
    /** Read status function. */
    flash_read_status_t read_status_func;
    /** Is busy function. */
    flash_is_busy_t is_busy_func;
    /** Write function. */
    flash_write_t write_func;
    /** Read function. */
    flash_read_t read_func;
    /** Read array function. */
    flash_read_array_t read_array_func;
    /** Write array function. */
    flash_write_array_t write_array_func;
    /** Flash size function. */
    flash_size_t flash_size_func;
    /** Flash block size function. */
    flash_block_size_t flash_block_size_func;
};
typedef struct flash_t flash_t;

/** Static variable. */
static flash_t flash;

void
flash_erase (flash_erase_cmd_t cmd, uint32_t start_addr)
{
    flash.erase_func (cmd, start_addr);
}

void
flash_send_command (flash_cmd_t cmd)
{
    flash.send_cmd_func (cmd);
}

uint8_t
flash_read_status (void)
{
    return flash.read_status_func ();
}

uint8_t
flash_is_busy (void)
{
    return flash.is_busy_func ();
}

void
flash_write (uint32_t addr, uint8_t data)
{
    flash.write_func (addr, data);
}

uint8_t
flash_read (uint32_t addr)
{
    return flash.read_func (addr);
}

void
flash_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    flash.read_array_func (addr, buffer, length);
}

void
flash_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    flash.write_array_func (addr, data, length);
}

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
	    proto_send4b ('a', 0, addr >> 16, addr >> 8, addr);
	    proto_send ('r', args[4], buf);
	  }
      case FLASH_CMD_READ_BYTE:
	if (size == 4)
	  {
	    proto_send1b ('r', flash_read (addr));
	  }
	break;
      case FLASH_CMD_WRITE_BYTE:
	if (size == 5)
	  {
	    proto_send1b ('w', args[4]);
	    flash_write (addr, args[4]);
	  }
	break;
      case FLASH_CMD_WRITE:
	if ((size == 5)
	    && (args[4] <= sizeof(buf)))
	  {
	    flash_write_array (addr, buf, args[4]);
	    proto_send ('w', args[4], buf);
	  }
	break;
      }

    return error;
}

uint32_t
flash_size (void)
{
    return flash.flash_size_func ();
}

uint32_t
flash_block_size (void)
{
    return flash.flash_block_size_func ();
}

void
flash_init_sst (void)
{
    flash.erase_func = flash_sst_erase;
    flash.send_cmd_func = flash_sst_send_command;
    flash.read_status_func = flash_sst_read_status;
    flash.is_busy_func = flash_sst_is_busy;
    flash.write_func = flash_sst_write;
    flash.read_func = flash_sst_read;
    flash.read_array_func = flash_sst_read_array;
    flash.write_array_func = flash_sst_write_array;
    flash.flash_size_func = flash_sst_size;
    flash.flash_block_size_func = flash_sst_block_size;
}
