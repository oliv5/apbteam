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

/** Call back use to call the user function on the reception of a data. */
typedef void (*spi_recv_cb_t) (void *user_data);

struct spi_t
{
    /** Call back function. */
    spi_recv_cb_t recv_cb;
    /** user data on data reception. */
    void *recv_user_data;
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
spi_send(uint8_t *data, u8 length);

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

/** Function called by the interruption if an IT is requested.
 *
 */
void
spi_catch_it(void);

#endif /* spi_h */
