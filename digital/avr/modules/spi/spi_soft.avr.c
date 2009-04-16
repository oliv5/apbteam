/* spi_soft.avr.c - SPI driver using regular GPIO. */
/* avr.spi - SPI AVR module. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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
#include "modules/utils/utils.h"

#include "spi.h"

#if SPI0_DRIVER == SPI_DRIVER_SOFT || SPI1_DRIVER == SPI_DRIVER_SOFT

#if SPI0_DRIVER == SPI_DRIVER_SOFT
# define SPI_SCK_IO AC_SPI0_SOFT_SCK_IO
# define SPI_MOSI_IO AC_SPI0_SOFT_MOSI_IO
# define SPI_MISO_IO AC_SPI0_SOFT_MISO_IO
#else
# define SPI_SCK_IO AC_SPI1_SOFT_SCK_IO
# define SPI_MOSI_IO AC_SPI1_SOFT_MOSI_IO
# define SPI_MISO_IO AC_SPI1_SOFT_MISO_IO
#endif

/** SPI speed, SCK period (us). */
static uint8_t spi_speed;

void
spi_soft_init_ (uint8_t speed)
{
    /* Set ports. */
    IO_DDR (SPI_MISO_IO) &= ~IO_BV (SPI_MISO_IO);
    IO_PORT (SPI_MISO_IO) |= IO_BV (SPI_MISO_IO);
    IO_PORT (SPI_MOSI_IO) &= ~IO_BV (SPI_MOSI_IO);
    IO_DDR (SPI_MOSI_IO) |= IO_BV (SPI_MOSI_IO);
    IO_PORT (SPI_SCK_IO) &= ~IO_BV (SPI_SCK_IO);
    IO_DDR (SPI_SCK_IO) |= IO_BV (SPI_SCK_IO);
    /* Store speed. */
    spi_speed = speed;
}

void
spi_soft_send (uint8_t data)
{
    spi_soft_send_and_recv (data);
}

uint8_t
spi_soft_recv (void)
{
    return spi_soft_send_and_recv (0);
}

uint8_t
spi_soft_send_and_recv (uint8_t data)
{
    uint8_t recv, i, j;
    recv = 0;
    for (i = 0x80; i; i >>= 1)
      {
	/* Setup data. */
	if (data & i)
	    IO_PORT (SPI_MOSI_IO) |= IO_BV (SPI_MOSI_IO);
	else
	    IO_PORT (SPI_MOSI_IO) &= ~IO_BV (SPI_MOSI_IO);
	/* Delay with SCK low. */
	for (j = 0; j < spi_speed; j++)
	    utils_delay_us (0.5);
	/* SCK high, sample. */
	if (IO_PIN (SPI_MISO_IO) & IO_BV (SPI_MISO_IO))
	    recv |= i;
	IO_PORT (SPI_SCK_IO) |= IO_BV (SPI_SCK_IO);
	/* Delay with SCK high. */
	for (j = 0; j < spi_speed; j++)
	    utils_delay_us (0.5);
	/* SCK low. */
	IO_PORT (SPI_SCK_IO) &= ~IO_BV (SPI_SCK_IO);
      }
    return recv;
}

#endif /* SPI0_DRIVER == SPI_DRIVER_SOFT || SPI1_DRIVER == SPI_DRIVER_SOFT */
