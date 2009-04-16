#ifndef spi_h
#define spi_h
/* spi.h */
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
#include "io.h"

/* Configuration constants are used for all drivers, even if values come from
 * hardware driver. */

/** Operate as a slave. */
#define SPI_SLAVE 0
/** Operate as master. */
#define SPI_MASTER _BV (MSTR)

/** Output MSB first. */
#define SPI_MSB_FIRST 0
/** Output LSB first. */
#define SPI_LSB_FIRST _BV (DORD)

/** Clock polarity: rising edge is the leading one, SCK is low when idle. */
#define SPI_CPOL_RISING 0
/** Clock polarity: falling edge is the leading one, SCK is high when idle. */
#define SPI_CPOL_FALLING _BV (CPOL)
/** Clock phase: sample on leading edge. */
#define SPI_CPHA_SAMPLE 0
/** Clock phase: setup on leading edge. */
#define SPI_CPHA_SETUP _BV (CPHA)

/** SPI mode 0, sample on rising edge, setup on falling edge. */
#define SPI_MODE_0 (SPI_CPOL_RISING | SPI_CPHA_SAMPLE)
/** SPI mode 1, setup on rising edge, sample on falling edge. */
#define SPI_MODE_1 (SPI_CPOL_RISING | SPI_CPHA_SETUP)
/** SPI mode 2, sample on falling edge, setup on rising edge. */
#define SPI_MODE_2 (SPI_CPOL_FALLING | SPI_CPHA_SAMPLE)
/** SPI mode 3, setup on falling edge, sample on rising edge. */
#define SPI_MODE_3 (SPI_CPOL_FALLING | SPI_CPHA_SETUP)
/** SPI mode mask. */
#define SPI_MODE_MASK SPI_MODE_3

/* Define selected drivers. */
#define SPI_DRIVER_NONE '0'
#define SPI_DRIVER_HARD 'h'
#define SPI_DRIVER_SOFT 's'
#define SPI_DRIVER__(drv) SPI_DRIVER_ ## drv
#define SPI_DRIVER_(drv) SPI_DRIVER__ (drv)
#define SPI0_DRIVER SPI_DRIVER_ (AC_SPI0_DRIVER)
#define SPI1_DRIVER SPI_DRIVER_ (AC_SPI1_DRIVER)

/* Include drivers header. */
#if SPI0_DRIVER == SPI_DRIVER_HARD || SPI1_DRIVER == SPI_DRIVER_HARD
# include "spi_hard.h"
#endif
#if SPI0_DRIVER == SPI_DRIVER_SOFT || SPI1_DRIVER == SPI_DRIVER_SOFT
# include "spi_soft.h"
#endif

/* Map names to drivers. */
#if SPI0_DRIVER == SPI_DRIVER_HARD
# define spi_init spi_hard_init
# define spi_send spi_hard_send
# define spi_recv spi_hard_recv
# define spi_send_and_recv spi_hard_send_and_recv
#elif SPI0_DRIVER == SPI_DRIVER_SOFT
# define spi_init spi_soft_init
# define spi_send spi_soft_send
# define spi_recv spi_soft_recv
# define spi_send_and_recv spi_soft_send_and_recv
#endif
#if SPI1_DRIVER == SPI_DRIVER_HARD
# define spi1_init spi_hard_init
# define spi1_send spi_hard_send
# define spi1_recv spi_hard_recv
# define spi1_send_and_recv spi_hard_send_and_recv
#elif SPI1_DRIVER == SPI_DRIVER_SOFT
# define spi1_init spi_soft_init
# define spi1_send spi_soft_send
# define spi1_recv spi_soft_recv
# define spi1_send_and_recv spi_soft_send_and_recv
#endif

#endif /* spi_h */
