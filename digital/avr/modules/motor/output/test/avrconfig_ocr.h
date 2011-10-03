#ifndef avrconfig_ocr_h
#define avrconfig_ocr_h
/* avrconfig_ocr.h */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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

/* global */
/** AVR Frequency : 1000000, 1843200, 2000000, 3686400, 4000000, 7372800,
 * 8000000, 11059200, 14745600, 16000000, 18432000, 20000000. */
#define AC_FREQ 14745600

/* uart - UART module. */
/** Select hardware uart for primary uart: 0, 1 or -1 to disable. */
#define AC_UART0_PORT 0
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
#define AC_UART0_SEND_BUFFER_SIZE 32
/** Recv buffer size, should be power of 2 for RING mode. */
#define AC_UART0_RECV_BUFFER_SIZE 32
/** If the send buffer is full when putc:
 *  - DROP: drop the new byte.
 *  - WAIT: wait until there is room in the send buffer. */
#define AC_UART0_SEND_BUFFER_FULL DROP
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

/* proto - Protocol module. */
/** Maximum argument size. */
#define AC_PROTO_ARGS_MAX_SIZE 8
/** Callback function name. */
#define AC_PROTO_CALLBACK proto_callback
/** Putchar function name. */
#define AC_PROTO_PUTC uart0_putc
/** Support for quote parameter. */
#define AC_PROTO_QUOTE 1

/* motor/output - Output module. */
/** Use Output Compare PWM output. */
#define AC_OUTPUT_USE_PWM_OCR 1
/** Use Motor Power PWM output. */
#define AC_OUTPUT_USE_PWM_MP 0
/** Define module and module index for each output. */
#define AC_OUTPUT_LIST (pwm_ocr, 0), (pwm_ocr, 1)

/* motor/output/pwm_ocr - Output Compare PWM output module. */
/** For each output, define output parameters:
 *
 * (timer, ocr, pwm_io, dir_io[, brake_io])
 *
 * With:
 *  - timer: timer number (ex: 1 for TIMER1)
 *  - ocr: output compare (ex: A for output compare A)
 *  - mode: compare output mode (ex: 2, see datasheet)
 *  - pwm_io: corresponding io port (ex: B, 1)
 *  - dir_io: io port used for direction (ex: B, 2)
 *  - brake_io: optional io port used for brake (ex: B, 3)
 */
#define AC_OUTPUT_PWM_OCR_LIST \
    (1, A, 2, D,5, D,3, A,4), \
    (1, B, 2, D,4, D,2, A,5)
/** Clock select for each used timer. */
#define AC_OUTPUT_PWM_OCR_CS_1 0b0001
/** Waveform Generation Mode for each used timer. */
#define AC_OUTPUT_PWM_OCR_WGM_1 0b0011
/** Offset added to PWM value to compensate for H-bridge weakness. */
#define AC_OUTPUT_PWM_OCR_OFFSET 0x40

#endif /* avrconfig_ocr_h */
