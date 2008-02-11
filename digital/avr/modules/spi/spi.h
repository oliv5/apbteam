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

#define SPI_IT_ENABLE 0x80
#define SPI_IT_DISABLE 0x0
#define SPI_ENABLE 0x40
#define SPI_DISABLE 0x0
#define SPI_MSB_FIRST 0x00
#define SPI_LSB_FIRST 0x20
#define SPI_MASTER 0x10
#define SPI_SLAVE 0x00
#define SPI_CPOL_RISING 0x0
#define SPI_CPOL_FALLING 0x8
#define SPI_CPHA_SAMPLE 0x0
#define SPI_CPHA_SETUP 0x4

enum spi_fosc_t
{
    SPI_FOSC_DIV_4,
    SPI_FOSC_DIV16,
    SPI_FOSC_DIV64,
    SPI_FOSC_DIV128,
    SPI_FOSC_DIV2,
    SPI_FOSC_DIV8,
    SPI_FOSC_DIV32
};

/** Call back use to call the user function on the reception of a data. */
typedef void (*spi_recv_cb_t) (void *user_data, uint8_t data);

struct spi_t
{
    /** Call back function. */
    spi_recv_cb_t recv_cb;
    /** user data on data reception. */
    void *recv_user_data;
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
spi_init(uint8_t sprc, spi_recv_cb_t cb, void *user_data);

/** Uninitialise the SPI module.
 * Unused on the target
 */
void
spi_uninit (void);

/** Send a data to the Slave.
 * \param  data  the data to send
 * \param  length  the length of the data in bytes.
 */
void
spi_send(uint8_t *data, uint8_t length);

/** Receive a data from the SPI bus.
 * \return  the data received from the bus.
 */
uint8_t 
spi_recv(void);

/** Return the status register from the SPI driver.
 * \return  the status register value
 */
uint8_t
spi_status(void);

#endif /* spi_h */
