#ifndef spi_soft_h
#define spi_soft_h
/* spi_soft.h */
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

/* Do not be fooled by symbol names, theses are the supported modes (others
 * will trigger descriptive compilation errors): */
#define SPI_MASTER_not_supported /* supported */
#define SPI_MODE_0_not_supported /* supported */
#define SPI_MSB_FIRST_not_supported /* supported */

/** Initialise SPI driver.
 * - master: slave or master mode
 * - mode: clock polarity and phase
 * - order: byte order (MSB or LSB)
 * - speed: clock rate (SCK duration)
 *
 * Only mode supported is MASTER, MODE_0, MSB_FIRST.
 */
#define spi_soft_init(master, mode, order, speed) \
    master ## _not_supported \
    mode ## _not_supported \
    order ## _not_supported \
    spi_soft_init_ (speed)
void
spi_soft_init_ (uint8_t speed);

/** Send data. */
void
spi_soft_send (uint8_t data);

/** Receive data. */
uint8_t
spi_soft_recv (void);

/** Send and receive data. */
uint8_t
spi_soft_send_and_recv (uint8_t data);

#endif /* spi_soft_h */
