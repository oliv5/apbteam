/* spi.avr.c */
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

/** Initialise the SPI Control Register.
 * \param  spcr  the spcr register data to be store in the register.
 */
void
spi_init (uint8_t spcr)
{
    /* Master configuration. */
    if (spcr & _BV(MSTR))
      {
	SPI_PORT |= _BV(SPI_BIT_SS);
	SPI_DDR |= _BV(SPI_BIT_SS);

	SPI_DDR &= ~_BV(SPI_BIT_MISO);
	SPI_PORT &= ~_BV(SPI_BIT_MISO);
	
	SPI_PORT |= _BV(SPI_BIT_MOSI);
	SPI_DDR |= _BV(SPI_BIT_MOSI);

	SPI_PORT |= _BV(SPI_BIT_SCK);
	SPI_DDR |= _BV(SPI_BIT_SCK);
      }
    else
      {
	SPI_DDR &= ~_BV(SPI_BIT_SS);
	SPI_PORT |= _BV(SPI_BIT_SS);

	SPI_PORT |= _BV(SPI_BIT_MISO);
	SPI_DDR |= _BV(SPI_BIT_MISO);

	SPI_DDR &= ~_BV(SPI_BIT_MOSI);
	SPI_PORT &= ~_BV(SPI_BIT_MOSI);

	SPI_DDR &= ~_BV(SPI_BIT_SCK);
	SPI_PORT &= ~_BV(SPI_BIT_SCK);
      }

    SPCR = spcr;
}

/** Send a data to the Slave.
 * \param  data  the data to send
 */
void
spi_send (uint8_t data)
{
    spi_send_and_recv (data);
}

/** Receive a data from the SPI bus.
 * \return  the data received from the bus.
 */
uint8_t
spi_recv (void)
{
    return spi_send_and_recv (0);
}

/** Send and receive data.
 * \param  data  the data to send.
 * \return  the data received.
 */
uint8_t
spi_send_and_recv (uint8_t data)
{
    SPDR = data;
    /* Wait the end of the transmission. */
    while (!(SPSR & _BV(SPIF)))
	;
    /* Return data register. */
    return SPDR;
}

