#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h */
/* avr.isp - Serial programming AVR module. {{{
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

/* utils */
/** AVR Frequency : 1000000, 1843200, 2000000, 3686400, 4000000, 7372800,
 * 8000000, 11059200, 14745600, 16000000, 18432000, 20000000. */
#define AC_FREQ 8000000

/* isp - ISP module. */
/** Size of isp_frame buffer. */
#define AC_ISP_FRAME_BUFFER_SIZE 275
/** Should be implemented by the user to send a character. */
#define AC_ISP_FRAME_SEND_CHAR uart0_putc
/** Should be implemented by the user (isp_proto) to accept a frame. */
#define AC_ISP_FRAME_ACCEPT_FRAME isp_proto_accept
/** Should be implemented by the user to send a frame. */
#define AC_ISP_PROTO_SEND isp_frame_send_frame
/** Programmer signature. */
#define AC_ISP_PROTO_SIGNATURE "APBisp_2"
/** Programmer build number. */
#define AC_ISP_PROTO_BUILD_NUMBER 0x0100
/** Programmer hardware version. */
#define AC_ISP_PROTO_HW_VERSION 0x02
/** Programmer software version. */
#define AC_ISP_PROTO_SW_VERSION 0x0204
/** Should be implemented by the user to enable SPI programming:
 * - set RESET and SCK to low,
 * - power on, or if not possible, do a positive RESET pulse,
 * - enable SPI. */
#define AC_ISP_SPI_ENABLE isp_spi_enable
/** Should be implemented by the user to disable SPI programming:
 * - disable SPI,
 * - release RESET,
 * - power off if desired. */
#define AC_ISP_SPI_DISABLE isp_spi_disable
/** Should be implemented by the user to do a pulse on SCK.  This is used to
 * try to resynchronise. */
#define AC_ISP_SPI_SCK_PULSE isp_spi_sck_pulse
/** Should be implemented by the user to send and receive a byte using the SPI
 * bus. */
#define AC_ISP_SPI_TX isp_spi_tx

#endif /* avrconfig_h */
