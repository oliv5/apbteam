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
#include "flash_sst.h"
#include "modules/spi/spi.h"

uint8_t
flash_init (void)
{
    uint8_t rsp[3];
    uint8_t res;

    AC_FLASH_PORT |= _BV(AC_FLASH_BIT_SS);
    AC_FLASH_DDR |= _BV(AC_FLASH_BIT_SS);

    /* send the read-ID instruction. */
    spi_init (SPI_MASTER, SPI_CPOL_FALLING | SPI_CPHA_SETUP, SPI_MSB_FIRST,
	      SPI_FOSC_DIV16);

    FLASH_CS_ENABLE;
    spi_send (FLASH_SST_CMD_READ_ID);
    rsp[0] = spi_recv ();
    rsp[1] = spi_recv ();
    rsp[2] = spi_recv ();
    FLASH_CS_DISABLE;

    switch (rsp[0])
      {
      case FLASH_SST_MANUFACTURER_ID:
	flash_sst_init ();
	flash_init_sst ();
	res = 1;
	break;
      default:
	res = 0;
      }
    return res;
}
