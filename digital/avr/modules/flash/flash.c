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
#include "modules/proto/proto.h"
#include "modules/spi/spi.h"
#include "modules/utils/utils.h"

/** Flash access.
 * The flash contains an address of 21 bits in a range from 0x0-0x1fffff.
 * This function shall access the memory directly by the SPI.
 * \param  addr  the address to provide to the flash memory.
 */
void
flash_address (uint32_t addr)
{
    /* The address must be sent */
    spi_send ((addr >> 16) & 0x1f);
    spi_send (addr >> 8);
    spi_send (addr);
}

/** Erase the memory.
 * \param  erase_type  the erase type..
 * \param  start_addr  the start address.
 */
void
flash_erase (uint8_t cmd, uint32_t start_addr)
{
    flash_send_command (FLASH_WREN);

    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    /* send the command. */
    spi_send (cmd);

    /* verify if the cmd is the full erase. */
    if (cmd != FLASH_ERASE_FULL)
      {
        /* Send the start address */
        flash_address (start_addr);
      }
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);

    while (flash_is_busy());
}

/* Send a flash command to the flash memory (only a command).
 * \param  cmd  the command to send.
 */
void
flash_send_command (uint8_t cmd)
{
    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    spi_send (cmd);
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);
}


/** Poll the busy bit in the Software Status Register of the flash memory.
 * \return  the status register.
 */
uint8_t
flash_read_status (void)
{
    uint8_t res;

    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    spi_send (FLASH_RDSR);
    res = spi_recv();
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);

    return res;
}

/** Initialise the flash memory.
 * \return true if the flash is present, false otherwise.
 */
uint8_t
flash_init (void)
{
    uint8_t rsp[3];

    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);
    AC_FLASH_DDR |= _BV(AC_FLASH_BIT_SS);

    /* send the read-ID instruction. */
    spi_init (SPI_IT_DISABLE | SPI_ENABLE | SPI_MASTER | SPI_MSB_FIRST
              | SPI_MASTER | SPI_CPOL_FALLING | SPI_CPHA_SETUP
              | SPI_FOSC_DIV16);

    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    spi_send (FLASH_READ_ID);
    rsp[0] = spi_recv ();
    rsp[1] = spi_recv ();
    rsp[2] = spi_recv ();
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);

    if (rsp[0] != 0xBF)
        return 0;

    proto_send3b ('f',rsp[0], rsp[1], rsp[2]);

    if (flash_status_aai())
      {
        flash_send_command (FLASH_WEDI);
      }

    /* Enables the flash to be writable. */
    flash_send_command (FLASH_WREN);

    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    spi_send (FLASH_WRSR);
    spi_send (0);
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);

    /* Read the flash status. */
    proto_send1b ('s', flash_read_status());

    return 1;
}

/** Find the first writable sector.
 * \param  addr  the address to start the research.
 * \return  the address of the next sector.
 */
uint32_t
flash_first_sector (void)
{
    uint8_t rsp = 0;
    uint32_t addr;

    /* Search for the next address to start writing. */
    for (addr = 0;
         (rsp != 0xFF) && (addr < FLASH_ADDRESS_HIGH);
         addr += FLASH_PAGE_SIZE)
      {
        rsp = flash_read (addr);
      }

    return addr < (FLASH_ADDRESS_HIGH + 1) ?
        addr - FLASH_PAGE_SIZE : FLASH_ADDRESS_ERROR;
}

/** Write in the flash byte provided in parameter.
 * \param  data  the buffer to store the data.
 */
void
flash_write (uint32_t addr, uint8_t data)
{
    flash_send_command (FLASH_WREN);
    while (flash_is_busy ());

    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    /* Write instruction. */
    spi_send (FLASH_WRITE);
    flash_address (addr);
    spi_send (data);
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);

    /* Wait for the flash until it is busy */
    while (flash_is_busy());
}

/** Read the data at the address provided.
 * \return  the data read.
 */
uint8_t
flash_read (uint32_t addr)
{
    uint8_t data;

    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    /* Send the read instruction. */
    spi_send (FLASH_READ);
    flash_address (addr);
    data = spi_recv ();
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);
    return data;
}

/** Read a data from the flash memory from the address provided and for a
 * length of the number of bytes provided.
 * \param  address at which the data should be read.
 * \param  buffer  the buffer to fill with the read data.
 * \param  length  the length of the data to read.
 *
 */
void
flash_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    uint8_t i;

    AC_FLASH_PORT &= ~_BV(AC_FLASH_BIT_SS);
    spi_send (FLASH_READ);
    flash_address (addr);
    for (i = 0; i < length; i++)
      {
        buffer[i] = spi_recv ();
      }
    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);
}

/** Write in the flash byte provided in parameter.
 * \param  addr  the address to store the data.
 * \param  data  the array to store.
 * \param  length  the array length
 */
void
flash_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
      {
        flash_write (addr + i, data[i]);
      }
}

uint8_t
flash_log (uint8_t size, uint8_t *args)
{
    uint8_t buf[128];
    uint8_t status = 0x0;
    uint32_t addr;

    if (size >= 4)
	addr = (((uint32_t) args[1]) << 16)
	    | (((uint32_t) args[2]) << 8) | args[3];

    switch (args[0])
      {
      case FLASH_CMD_INIT:
	status = flash_init ();
	break;
      case FLASH_CMD_READ:
	if ((size == 5)
	    && (args[4] < sizeof(buf)))
	  {
	    flash_read_array (addr, buf, args[4]);
	    proto_send ('r', args[4], buf);
	    status = 0x1;
	  }
	else if (size == 4)
	  {
	    proto_send1b ('r', flash_read (addr));
	    status = 0x1;
	  }
	break;
      default:
	status = 0x0;
      }

    return status;
}
