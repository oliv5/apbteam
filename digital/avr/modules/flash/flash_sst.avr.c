/* flash_sst.avr.c */
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
#include "flash_sst.h"
#include "modules/proto/proto.h"
#include "modules/spi/spi.h"
#include "modules/utils/utils.h"

/** Return the AAI status flag of the register.
  * \return  the AAI status.
  */
extern inline uint8_t
flash_sst_status_aai (void)
{
    return flash_sst_read_status () >> 6;
}

void
flash_sst_init (void)
{
    if (flash_sst_status_aai())
	flash_sst_send_command (FLASH_SST_CMD_WEDI);
    /* Enables the flash to be writable. */
    flash_sst_send_command (FLASH_SST_CMD_WREN);
    FLASH_CS_ENABLE;
    spi_send (FLASH_SST_CMD_WRSR);
    spi_send (0);
    FLASH_CS_DISABLE;
}

/** Flash access.
  * The flash contains an address of 21 bits in a range from 0x0-0x1fffff.
  * This function shall access the memory directly by the SPI.
  * \param  addr  the address to provide to the flash memory.
  */
static inline void
flash_sst_address (uint32_t addr)
{
    if (FLASH_SST_ADDR_IS_VALID (addr))
      {
	/* The address must be sent */
	spi_send ((addr >> 16) & 0x1f);
	spi_send (addr >> 8);
	spi_send (addr);
      }
}

void
flash_sst_erase (flash_erase_cmd_t cmd, uint32_t start_addr)
{
    uint8_t sst_cmd;
    switch (cmd)
      {
      case FLASH_ERASE_PAGE:
	sst_cmd = FLASH_SST_ERASE_4K;
	break;
      case FLASH_ERASE_SECTOR:
	sst_cmd = FLASH_SST_ERASE_32K;
	break;
      case FLASH_ERASE_BLOCK:
	sst_cmd = FLASH_SST_ERASE_64K;
	break;
      default:
    sst_cmd = FLASH_SST_ERASE_FULL;
      }
    flash_sst_send_command (FLASH_SST_CMD_WREN);
    FLASH_CS_ENABLE;
    /* send the command. */
    spi_send (cmd);
    /* verify if the cmd is the full erase. */
    if (sst_cmd != FLASH_SST_ERASE_FULL)
        /* Send the start address */
        flash_sst_address (start_addr);
    FLASH_CS_DISABLE;
    while (flash_sst_is_busy());
}

void
flash_sst_send_command (flash_cmd_t cmd)
{
    uint8_t sst_cmd;
    switch (cmd)
      {
      case FLASH_CMD_READ_BYTE:
      case FLASH_CMD_READ:
	sst_cmd = FLASH_SST_CMD_READ;
	break;
      case FLASH_CMD_WRITE_BYTE:
      case FLASH_CMD_WRITE:
	sst_cmd = FLASH_SST_CMD_WRITE;
	break;
      default:
	return;
      }
    FLASH_CS_ENABLE;
    spi_send (cmd);
    FLASH_CS_DISABLE;
}

uint8_t
flash_sst_read_status (void)
{
    uint8_t res;
    FLASH_CS_ENABLE;
    spi_send (FLASH_SST_CMD_RDSR);
    res = spi_recv();
    FLASH_CS_DISABLE;
    return res;
}

uint8_t
flash_sst_is_busy (void)
{
    return flash_sst_read_status () & 0x1;
}

void
flash_sst_write (uint32_t addr, uint8_t data)
{
    if (FLASH_SST_ADDR_IS_VALID (addr))
      {
	while (flash_sst_is_busy ());
	flash_sst_send_command (FLASH_SST_CMD_WREN);
	FLASH_CS_ENABLE;
	/* Write instruction. */
	spi_send (FLASH_SST_CMD_WRITE);
	flash_sst_address (addr);
	spi_send (data);
	FLASH_CS_DISABLE;
	/* Wait for the flash until it is busy */
	while (flash_sst_is_busy());
      }
}

uint8_t
flash_sst_read (uint32_t addr)
{
    if (FLASH_SST_ADDR_IS_VALID (addr))
      {
	uint8_t data;
	while (flash_sst_is_busy ());
	FLASH_CS_ENABLE;
	/* Send the read instruction. */
	spi_send (FLASH_SST_CMD_READ);
	flash_sst_address (addr);
	data = spi_recv ();
	FLASH_CS_DISABLE;
	while (flash_sst_is_busy ());
	return data;
      }
    return 0xFF;
}

void
flash_sst_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    if (FLASH_SST_ADDR_IS_VALID (addr))
      {
	uint8_t i;
	while (flash_sst_is_busy ());
	FLASH_CS_ENABLE;
	spi_send (FLASH_SST_CMD_READ);
	flash_sst_address (addr);
	for (i = 0; i < length; i++)
	    buffer[i] = spi_recv ();
	FLASH_CS_DISABLE;
	while (flash_sst_is_busy ());
      }
}

void
flash_sst_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    if (FLASH_SST_ADDR_IS_VALID (addr))
      {
	uint32_t i;
	for (i = 0; i < length; i++)
	    flash_sst_write (addr + i, data[i]);
      }
}
