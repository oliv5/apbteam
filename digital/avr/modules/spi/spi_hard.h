#ifndef spi_hard_h
#define spi_hard_h
/* spi_hard.h */
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

/** Hardware available clock rates. */
enum spi_fosc_t
{
    SPI_FOSC_DIV2 = (_BV (SPI2X) << 8),
    SPI_FOSC_DIV4 = 0,
    SPI_FOSC_DIV8 = (_BV (SPI2X) << 8) | _BV (SPR0),
    SPI_FOSC_DIV16 = _BV (SPR0),
    SPI_FOSC_DIV32 = (_BV (SPI2X) << 8) | _BV (SPR1),
    SPI_FOSC_DIV64 = _BV (SPR1),
    SPI_FOSC_DIV128 = _BV (SPR1) | _BV (SPR0),
};

/** Initialise SPI driver.
 * - master: slave or master mode
 * - mode: clock polarity and phase
 * - order: byte order (MSB or LSB)
 * - speed: clock rate (use SPI_FOSC_DIV*) */
#define spi_hard_init(master, mode, order, speed) \
    spi_hard_init_ ((master) | (mode) | (order) | ((speed) & 0xff), \
		    (speed) >> 8)
void
spi_hard_init_ (uint8_t spcr, uint8_t spi2x);

/** Send data. */
void
spi_hard_send (uint8_t data);

/** Receive data. */
uint8_t
spi_hard_recv (void);

/** Send and receive data. */
uint8_t
spi_hard_send_and_recv (uint8_t data);

#endif /* spi_hard_h */
