/* spi.c */
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
#include "io.h"
#include "common.h"
#include "spi.h"

#define SPI_DIRVER_SIGNAL SIGNAL_SPI

/** For host */
#ifdef HOST
static uint8_t SPCR;
static uint8_t SPDR;
static uint8_t SPSR;
static uint8_t SPIF;
#endif

/** Spi driver context. */
static spi_t spi_global;

/** Initialise the SPI Control Register.
 * \param  sprc  the sprc register data to be store in the register.
 * \param  cb  the function call back to call the user which receive the data.
 * \param  user_data  the user data to be provieded in the function call back.
 */
void
spi_init(uint8_t sprc)
{
    /* Master configuration. */
    if (sprc & _BV(MSTR))
      {
	SPI_DDR &= ~_BV(SPI_BIT_MISO);
	SPI_PORT &= ~_BV(SPI_BIT_MISO);
	
	SPI_PORT |= _BV(SPI_BIT_MOSI);
	SPI_DDR |= _BV(SPI_BIT_MOSI);

	SPI_PORT |= _BV(SPI_BIT_SCK);
	SPI_DDR |= _BV(SPI_BIT_SCK);
      }
    else
      {
	SPI_PORT |= _BV(SPI_BIT_MISO);
	SPI_DDR |= _BV(SPI_BIT_MISO);

	SPI_DDR &= ~_BV(SPI_BIT_MOSI);
	SPI_PORT &= ~_BV(SPI_BIT_MOSI);

	SPI_DDR &= ~_BV(SPI_BIT_SCK);
	SPI_PORT &= ~_BV(SPI_BIT_SCK);
      }

    SPCR = sprc;
    spi_global.interruption = SPCR & _BV(SPIE);
}

/** Send a data to the Slave.
 * \param  data  the data to send
 */
void
spi_send(uint8_t data)
{
    // Wait the end of the transmission.
    while(!(SPSR & _BV(SPIF))); 

    SPDR = data;
}

/** Receive a data from the SPI bus.
 * \return  the data received from the bus.
 */
uint8_t
spi_recv(void)
{
    /* Wait for reception complete */
    while(!(SPSR & _BV(SPIF))); 
    
    /* Return data register */
    return SPDR;
}

/** Send and receive data.
  * \param  data  the data to send.
  * \return  the data received.
  */
uint8_t
spi_send_and_recv (uint8_t data)
{
    // Wait the end of the transmission.
    while(!(SPSR & _BV(SPIF))); 

    SPDR = data;
    return SPDR;
}

/** Return the status register from the SPI driver.
 * \return  the status register value
 */
uint8_t
spi_status(void)
{
    return SPSR;
}

