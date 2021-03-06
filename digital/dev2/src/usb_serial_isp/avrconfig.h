#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h */
/* usb - USB device module using LUFA. {{{
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
#error "please check frequency"
#define AC_FREQ 8000000

/* uart - UART module. */
/** Select hardware uart for primary uart: 0, 1 or -1 to disable. */
#define AC_UART0_PORT 1
/** Baudrate: 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 76800,
 * 115200, 230400, 250000, 500000, 1000000. */
#define AC_UART0_BAUDRATE 38400
/** Send mode:
 *  - POLLING: no interrupts.
 *  - RING: interrupts, ring buffer. */
#define AC_UART0_SEND_MODE RING
/** Recv mode, same as send mode. */
#define AC_UART0_RECV_MODE RING
/** Character size: 5, 6, 7, 8, 9 (only 8 implemented). */
#define AC_UART0_CHAR_SIZE 8
/** Parity : ODD, EVEN, NONE. */
#define AC_UART0_PARITY EVEN
/** Stop bits : 1, 2. */
#define AC_UART0_STOP_BITS 1
/** Send buffer size, should be power of 2 for RING mode. */
#define AC_UART0_SEND_BUFFER_SIZE 16
/** Recv buffer size, should be power of 2 for RING mode. */
#define AC_UART0_RECV_BUFFER_SIZE 32
/** If the send buffer is full when putc:
 *  - DROP: drop the new byte.
 *  - WAIT: wait until there is room in the send buffer. */
#define AC_UART0_SEND_BUFFER_FULL WAIT
/** In HOST compilation:
 *  - STDIO: use stdin/out.
 *  - PTS: use pseudo terminal. */
#define AC_UART0_HOST_DRIVER STDIO
/** Same thing for secondary port. */
#define AC_UART1_PORT -1
#define AC_UART1_BAUDRATE 115200
#define AC_UART1_SEND_MODE RING
#define AC_UART1_RECV_MODE RING
#define AC_UART1_CHAR_SIZE 8
#define AC_UART1_PARITY EVEN
#define AC_UART1_STOP_BITS 1
#define AC_UART1_SEND_BUFFER_SIZE 32
#define AC_UART1_RECV_BUFFER_SIZE 32
#define AC_UART1_SEND_BUFFER_FULL WAIT
#define AC_UART1_HOST_DRIVER PTS

/* spi - SPI module. */
/** Select driver: HARD, SOFT, or NONE. */
#define AC_SPI0_DRIVER SOFT
/** For software SPI, specify SCK IO port. */
#define AC_SPI0_SOFT_SCK_IO D, 5
/** For software SPI, specify MOSI IO port. */
#define AC_SPI0_SOFT_MOSI_IO D, 7
/** For software SPI, specify MISO IO port. */
#define AC_SPI0_SOFT_MISO_IO D, 6
/** Same thing for an optionnal second SPI driver. */
#define AC_SPI1_DRIVER NONE

/* isp - ISP module. */
/** Size of isp_frame buffer. */
#define AC_ISP_FRAME_BUFFER_SIZE 275
/** Should be implemented by the user to send a character. */
#define AC_ISP_FRAME_SEND_CHAR usb_isp_send_char
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
#define AC_ISP_SPI_ENABLE usb_isp_spi_enable
/** Should be implemented by the user to disable SPI programming:
 * - disable SPI,
 * - release RESET,
 * - power off if desired. */
#define AC_ISP_SPI_DISABLE usb_isp_spi_disable
/** Should be implemented by the user to do a pulse on SCK.  This is used to
 * try to resynchronise. */
#define AC_ISP_SPI_SCK_PULSE usb_isp_spi_sck_pulse
/** Should be implemented by the user to send and receive a byte using the SPI
 * bus. */
#define AC_ISP_SPI_TX usb_isp_spi_tx

/* usb */
#include "modules/usb/lufaconfig.h"

/* dev2 */
/** Set to 1 if ISP and serial pins are connected together. */
#define AC_DEV2_SERIAL_ISP_SHARED 0

#endif /* avrconfig_h */
