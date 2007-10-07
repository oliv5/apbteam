#ifndef uart_common_h
#define uart_common_h
/* uart_common.h - UART module common part. */
/* avr.uart - UART AVR module. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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

/* This symbol should be 0 or 1. */
#ifndef UART_N
# error "uart: UART_N missing"
#endif

/* Utility macros. */
#define PASTE4_SUB(a, b, c, d) a ## b ## c ## d
#define PASTE4(a, b, c, d) PASTE4_SUB (a, b, c, d)

/* Port selection macros. */
#define AC_UART(x) PASTE4(AC_UART, UART_N, _, x)
#define uart(x) PASTE4(uart, UART_N, _, x)

/* Define uart symbols. */
#define uart_error uart (error)
#define uart_error_code uart (error_code)
#define uart_getc uart (getc)
#define uart_init uart (init)
#define uart_poll uart (poll)
#define uart_putc uart (putc)
#define uart_recv_buffer uart (recv_buffer)
#define uart_recv_head uart (recv_head)
#define uart_recv_tail uart (recv_tail)
#define uart_send_buffer uart (send_buffer)
#define uart_send_head uart (send_head)
#define uart_send_tail uart (send_tail)

/* Unimplemented features. */
#if AC_UART (CHAR_SIZE) != 8
# error "uart: char size != 8 not implemented"
#endif

/* Test that everythings is configured. */
#if !defined (AC_UART0_PORT) \
    || !defined (AC_UART0_BAUDRATE) \
    || !defined (AC_UART0_SEND_MODE) \
    || !defined (AC_UART0_RECV_MODE) \
    || !defined (AC_UART0_CHAR_SIZE) \
    || !defined (AC_UART0_PARITY) \
    || !defined (AC_UART0_STOP_BITS) \
    || !defined (AC_UART0_SEND_BUFFER_SIZE) \
    || !defined (AC_UART0_RECV_BUFFER_SIZE) \
    || !defined (AC_UART0_SEND_BUFFER_FULL) \
    || !defined (AC_UART0_HOST_DRIVER) \
    || !defined (AC_UART1_PORT) \
    || !defined (AC_UART1_BAUDRATE) \
    || !defined (AC_UART1_SEND_MODE) \
    || !defined (AC_UART1_RECV_MODE) \
    || !defined (AC_UART1_CHAR_SIZE) \
    || !defined (AC_UART1_PARITY) \
    || !defined (AC_UART1_STOP_BITS) \
    || !defined (AC_UART1_SEND_BUFFER_SIZE) \
    || !defined (AC_UART1_RECV_BUFFER_SIZE) \
    || !defined (AC_UART1_SEND_BUFFER_FULL) \
    || !defined (AC_UART1_HOST_DRIVER) \
    || AC_UART0_PORT == AC_UART1_PORT
# error "uart: error in configuration"
#endif

#endif /* uart_common_h */
