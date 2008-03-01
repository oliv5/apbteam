#ifndef spi_h
#define spi_h
/* spi.h */
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

#define SPI_DDR DDRB
#define SPI_PORT PORTB

#if defined  (__AVR_ATmega128__)
#define SPI_BIT_SCK 1
#define SPI_BIT_MOSI 2
#define SPI_BIT_MISO 3
#elif defined (__AVR_ATmega16__)
#define SPI_BIT_SCK 7
#define SPI_BIT_MOSI 5
#define SPI_BIT_MISO 6
#else
#error "Not implemented"
#endif

#define SPI_IT_ENABLE _BV(SPIE) 
#define SPI_IT_DISABLE 0x0
#define SPI_ENABLE _BV(SPE)
#define SPI_DISABLE 0x0
#define SPI_MSB_FIRST 0x00
#define SPI_LSB_FIRST _BV(DORD)
#define SPI_MASTER _BV(MSTR)
#define SPI_SLAVE 0x00
#define SPI_CPOL_RISING 0x0
#define SPI_CPOL_FALLING _BV(CPOL)
#define SPI_CPHA_SAMPLE 0x0
#define SPI_CPHA_SETUP _BV(CPHA)

enum spi_fosc_t
{
    SPI_FOSC_DIV4,
    SPI_FOSC_DIV16,
    SPI_FOSC_DIV64,
    SPI_FOSC_DIV128,
    SPI_FOSC_DIV2,
    SPI_FOSC_DIV8,
    SPI_FOSC_DIV32
};

struct spi_t
{
    /** interruption status. */
    int8_t interruption;
};
typedef struct spi_t spi_t;

/** Initialise the SPI Control Register.
 * \param  sprc  the sprc register data to be store in the register.
 * \param  cb  the function call back to call the user which receive the data.
 * \param  user_data  the user data to be provieded in the function call back.
 */
void
spi_init(uint8_t sprc);

/** Uninitialise the SPI module.
 * Unused on the target
 */
void
spi_uninit (void);

/** Send a data to the Slave.
 * \param  data  the data to send
 */
void
spi_send(uint8_t data);

/** Receive a data from the SPI bus.
 * \return  the data received from the bus.
 */
uint8_t 
spi_recv(void);

/** Send and receive data.
  * \param  data  the data to send.
  * \return  the data received.
  */
uint8_t
spi_send_and_recv (uint8_t data);

/** Return the status register from the SPI driver.
 * \return  the status register value
 */
uint8_t
spi_status(void);

#endif /* spi_h */
