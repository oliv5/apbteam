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
spi_init(uint8_t sprc, spi_recv_cb_t cb, void *user_data)
{
    SPCR = sprc;
    spi_global.recv_cb = cb;
    spi_global.recv_user_data = user_data;
    spi_global.interruption = sprc >> 7;
}

/** Send a data to the Slave.
 * \param  data  the data to send
 * \param  length  the length of the data in bytes.
 */
void
spi_send(uint8_t *data, uint8_t length)
{
    uint8_t i;
    // enables the SPI if not enabled.
    SPCR |= SPI_ENABLE;

    for ( i = 0; i < length; i++)
    {
      // Insert the data in the data register, the SPI will begin to send it
      // automatically.
      SPDR = data[i];

      if (!spi_global.interruption)
	{
	  // Wait the end of the transmission.
	  while(!(SPSR & (1<<SPIF))); 
	}
    }
}

/** Receive a data from the SPI bus.
 * \return  the data received from the bus.
 */
uint8_t
spi_recv(void)
{
    /* Wait for reception complete */
    while(!(SPSR & (1<<SPIF)));
    
    /* Return data register */
    return SPDR;
}

/** Receive a date from the SPI bus from the address provided by parameters.
  * \param  addr  the address from which the data shall be read
  * \return  the data at the address requested.
  */
uint8_t
spi_recv_from (uint8_t addr)
{
    SPDR = addr;

    return spi_recv();
}

/** Return the status register from the SPI driver.
 * \return  the status register value
 */
uint8_t
spi_status(void)
{
    return SPSR;
}

/** 
 * Function called by the interruption if an IT is requested.
 */
SIGNAL (SIG_SPI)
{
    // call the call back.
    (*spi_global.recv_cb) (spi_global.recv_user_data, SPDR);
}

