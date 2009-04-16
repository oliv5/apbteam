#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h - SPI module configuration template. */
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

/* spi - SPI module. */
/** Select driver: HARD, SOFT, or NONE. */
#define AC_SPI0_DRIVER HARD
/** For software SPI, specify SCK IO port. */
#define AC_SPI0_SOFT_SCK_IO D, 5
/** For software SPI, specify MOSI IO port. */
#define AC_SPI0_SOFT_MOSI_IO D, 7
/** For software SPI, specify MISO IO port. */
#define AC_SPI0_SOFT_MISO_IO D, 6
/** Same thing for an optionnal second SPI driver. */
#define AC_SPI1_DRIVER NONE
#define AC_SPI1_SOFT_SCK_IO
#define AC_SPI1_SOFT_MOSI_IO
#define AC_SPI1_SOFT_MISO_IO

#endif /* avrconfig_h */
