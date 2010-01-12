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

uint8_t
flash_init (void)
{
    uint8_t rsp[3];

    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);
    AC_FLASH_DDR |= _BV(AC_FLASH_BIT_SS);

    /* send the read-ID instruction. */
    spi_init (SPI_MASTER, SPI_CPOL_FALLING | SPI_CPHA_SETUP, SPI_MSB_FIRST,
	      SPI_FOSC_DIV16);

    FLASH_CS_ENABLE;
    spi_send (FLASH_READ_ID);
    rsp[0] = spi_recv ();
    rsp[1] = spi_recv ();
    rsp[2] = spi_recv ();
    FLASH_CS_DISABLE;

    if (rsp[0] != 0xBF)
        return 0;

    if (flash_status_aai())
      {
        flash_send_command (FLASH_WEDI);
      }

    /* Enables the flash to be writable. */
    flash_send_command (FLASH_WREN);

    FLASH_CS_ENABLE;
    spi_send (FLASH_WRSR);
    spi_send (0);
    FLASH_CS_DISABLE;

    return 1;
}

void
flash_address (uint32_t addr)
{
    /* The address must be sent */
    spi_send ((addr >> 16) & 0x1f);
    spi_send (addr >> 8);
    spi_send (addr);
}

void
flash_erase (uint8_t cmd, uint32_t start_addr)
{
    flash_send_command (FLASH_WREN);

    FLASH_CS_ENABLE;
    /* send the command. */
    spi_send (cmd);

    /* verify if the cmd is the full erase. */
    if (cmd != FLASH_ERASE_FULL)
      {
        /* Send the start address */
        flash_address (start_addr);
      }
    FLASH_CS_DISABLE;

    while (flash_is_busy());
}

void
flash_send_command (flash_cmd_t cmd)
{
    FLASH_CS_ENABLE;
    spi_send (cmd);
    FLASH_CS_DISABLE;
}

uint8_t
flash_read_status (void)
{
    uint8_t res;

    FLASH_CS_ENABLE;
    spi_send (FLASH_RDSR);
    res = spi_recv();
    FLASH_CS_DISABLE;

    return res;
}

void
flash_write (uint32_t addr, uint8_t data)
{
    while (flash_is_busy ());
    flash_send_command (FLASH_WREN);

    FLASH_CS_ENABLE;
    /* Write instruction. */
    spi_send (FLASH_WRITE);
    flash_address (addr);
    spi_send (data);
    FLASH_CS_DISABLE;

    /* Wait for the flash until it is busy */
    while (flash_is_busy());
}

uint8_t
flash_read (uint32_t addr)
{
    uint8_t data;

    while (flash_is_busy ());
    FLASH_CS_ENABLE;
    /* Send the read instruction. */
    spi_send (FLASH_READ);
    flash_address (addr);
    data = spi_recv ();
    FLASH_CS_DISABLE;
    while (flash_is_busy ());
    return data;
}

void
flash_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    uint8_t i;

    while (flash_is_busy ());
    FLASH_CS_ENABLE;
    spi_send (FLASH_READ);
    flash_address (addr);
    for (i = 0; i < length; i++)
      {
        buffer[i] = spi_recv ();
      }
    FLASH_CS_DISABLE;
    while (flash_is_busy ());
}

void
flash_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
      {
        flash_write (addr + i, data[i]);
      }
}
