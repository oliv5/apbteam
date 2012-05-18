#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#define AC_FREQ 16000000

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
#define AC_UART0_SEND_BUFFER_SIZE 32
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

/* proto - Protocol module. */
/** Maximum argument size. */
#define AC_PROTO_ARGS_MAX_SIZE 12
/** Callback function name. */
#define AC_PROTO_CALLBACK proto_callback
/** Putchar function name. */
#define AC_PROTO_PUTC uart0_putc
/** Support for quote parameter. */
#define AC_PROTO_QUOTE 1

/* twi - TWI module. */
/** Driver to implement TWI: HARD, SOFT, or USI. */
#define AC_TWI_DRIVER HARD
/** Do not use interrupts. */
#define AC_TWI_NO_INTERRUPT 0
/** TWI frequency, should really be 100 kHz. */
#define AC_TWI_FREQ 100000
/** Enable slave part. */
#define AC_TWI_SLAVE_ENABLE 0
/** Enable master part. */
#define AC_TWI_MASTER_ENABLE 1
/** Master transfer completion callback, optionally defined by the user, called
 * at end of master transfer. */
#undef AC_TWI_MASTER_DONE
/** Use internal pull up. */
#define AC_TWI_PULL_UP 0

/* usdist - Analog US distance sensor. */
/** Number of sensors. */
#define AC_USDIST_NB 4
/** Measuring period, in number of update call. */
#define AC_USDIST_PERIOD 1
/** List of space separated sensor definition, see usdist.h. */
#define AC_USDIST_SENSORS \
    USDIST_SENSOR (0, A, 0) \
    USDIST_SENSOR (1, A, 1) \
    USDIST_SENSOR (2, A, 2) \
    USDIST_SENSOR (3, A, 3)

/* path - Path finding module. */
/** Report path found for debug. */
#define AC_PATH_REPORT defined (HOST)
/** Report function name. */
#define AC_PATH_REPORT_CALLBACK simu_send_path
/** Number of possible obstacles. */
#define AC_PATH_OBSTACLES_NB 2

/* astar - A* path finding module. */
/** Neighbor callback. */
#define AC_ASTAR_NEIGHBOR_CALLBACK path_astar_neighbor_callback
/** Heuristic callback. */
#define AC_ASTAR_HEURISTIC_CALLBACK path_astar_heuristic_callback

/* io-hub - io/ai board. */
/** TWI address of the io board. */
#define AC_IO_TWI_ADDRESS 8
/** PWM setting. */
#undef AC_IOHUB_PWM
/** Use asserv board. */
#define AC_AI_TWI_MASTER_ASSERV 1
/** Use mimot board. */
#define AC_AI_TWI_MASTER_MIMOT 1
/** Use beacon board. */
#define AC_AI_TWI_MASTER_BEACON 0

/* beacon. */
/** Number of beacon positions. */
#define AC_BEACON_POSITION_NB 2

/* asserv. */
/** Number of auxiliary motors. */
#define AC_ASSERV_AUX_NB 0

#endif /* avrconfig_h */
