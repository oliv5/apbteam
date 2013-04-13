#ifndef gpio_proto_h
#define gpio_proto_h
/* gpio_proto.h */
/* dev2 - Multi-purpose development board using USB and Ethernet. {{{
 *
 * Copyright (C) 2013 Nicolas Schodet
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

/* All communications are little endian. */

/* Reset communication and set all pins as input. */
#define GPIO_OP_RESET_SYNC 0xa5
/* Set port width (1 byte argument) and period (4 bytes, in ns unit).
 * Port width will determine the size of a port argument (1 byte for 8, 2
 * bytes for 16...). Period is the pause between two output changes, 0 means
 * as fast as possible. */
#define GPIO_OP_SETUP 0xaa

/* Set complete port direction, 1 is output. */
#define GPIO_OP_DIR 0xb0
/* Set direction as output for selected pins. */
#define GPIO_OP_DIR_OUT 0xb1
/* Set direction as input for selected pins. */
#define GPIO_OP_DIR_IN 0xb2
/* Set complete output port value. */
#define GPIO_OP_OUT 0xb4
/* Set pins in output port. */
#define GPIO_OP_OUT_SET 0xb5
/* Reset pins in output port. */
#define GPIO_OP_OUT_RESET 0xb6
/* Toggle pins in output port. */
#define GPIO_OP_OUT_TOGGLE 0xb7
/* Change pins (mask as first argument, value as second argument). */
#define GPIO_OP_OUT_CHANGE 0xb8
/* Request port input. */
#define GPIO_OP_IN 0xb9

/* Send serial data (data mask as first argument, clock mask as second
 * argument, data length as third argument, then a number of data lsb first).
 * This will: set data, toggle clock twice, start again. */
#define GPIO_OP_SERIAL_OUT 0xc0
/* Receive serial data (data mask as first argument, clock mask as second
 * argument, data length as third argument, will response with received data).
 * This will: read data, toggle clock twice, start again. */
#define GPIO_OP_SERIAL_IN 0xc1


#endif /* gpio_proto_h */
