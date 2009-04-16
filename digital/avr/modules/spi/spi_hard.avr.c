/* spi_hard.avr.c - SPI driver using hardware implementation. */
/* avr.spi - SPI AVR module. {{{
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

#include "spi.h"

#if SPI0_DRIVER == SPI_DRIVER_HARD || SPI1_DRIVER == SPI_DRIVER_HARD

#if defined  (__AVR_ATmega128__) \
    || defined (__AVR_ATmega64__) \
    || defined (__AVR_AT90USB162__)
# define SPI_SS_IO B, 0
# define SPI_SCK_IO B, 1
# define SPI_MOSI_IO B, 2
# define SPI_MISO_IO B, 3
#elif defined (__AVR_ATmega16__)
# define SPI_SS_IO B, 4
# define SPI_SCK_IO B, 7
# define SPI_MOSI_IO B, 5
# define SPI_MISO_IO B, 6
#else
# error "spi: not implemented on this chip"
#endif

void
spi_hard_init_ (uint8_t spcr, uint8_t spi2x)
{
    if (spcr & SPI_MASTER)
      {
	/* Master configuration. */
	/* SS must be output or SPI could switch to slave mode. */
	IO_PORT (SPI_SS_IO) |= IO_BV (SPI_SS_IO);
	IO_DDR (SPI_SS_IO) |= IO_BV (SPI_SS_IO);
	IO_DDR (SPI_MISO_IO) &= ~IO_BV (SPI_MISO_IO);
	IO_PORT (SPI_MISO_IO) &= ~IO_BV (SPI_MISO_IO);
	IO_PORT (SPI_MOSI_IO) |= IO_BV (SPI_MOSI_IO);
	IO_DDR (SPI_MOSI_IO) |= IO_BV (SPI_MOSI_IO);
	IO_PORT (SPI_SCK_IO) |= IO_BV (SPI_SCK_IO);
	IO_DDR (SPI_SCK_IO) |= IO_BV (SPI_SCK_IO);
      }
    else
      {
	/* Slave configuration, will set MISO later. */
	IO_DDR (SPI_SS_IO) &= ~IO_BV (SPI_SS_IO);
	IO_PORT (SPI_SS_IO) |= IO_BV (SPI_SS_IO);
	IO_DDR (SPI_MOSI_IO) &= ~IO_BV (SPI_MOSI_IO);
	IO_PORT (SPI_MOSI_IO) &= ~IO_BV (SPI_MOSI_IO);
	IO_DDR (SPI_SCK_IO) &= ~IO_BV (SPI_SCK_IO);
	IO_PORT (SPI_SCK_IO) &= ~IO_BV (SPI_SCK_IO);
      }
    SPCR = spcr | _BV (SPE);
    SPSR = spi2x;
    if (!(spcr & SPI_MASTER))
      {
	/* Slave configuration, set MISO now or else it will be an output for
	 * a short time until SPI is activate. */
	IO_PORT (SPI_MISO_IO) |= IO_BV (SPI_MISO_IO);
	IO_DDR (SPI_MISO_IO) |= IO_BV (SPI_MISO_IO);
      }
}

void
spi_hard_send (uint8_t data)
{
    spi_hard_send_and_recv (data);
}

uint8_t
spi_hard_recv (void)
{
    return spi_hard_send_and_recv (0);
}

uint8_t
spi_hard_send_and_recv (uint8_t data)
{
    SPDR = data;
    /* Wait the end of the transmission. */
    while (!(SPSR & _BV(SPIF)))
	;
    /* Return data register. */
    return SPDR;
}

#endif /* SPI0_DRIVER == SPI_DRIVER_HARD || SPI1_DRIVER == SPI_DRIVER_HARD */
