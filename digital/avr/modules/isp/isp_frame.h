#ifndef isp_frame_h
#define isp_frame_h
/* isp_frame.h */
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

/** The isp_frame sub module transfers data between a character oriented device
 * (uart, usb bulk, tcp socket) and a framed device (isp_proto) to support
 * implementation of AVR068. */

/** Should be implemented by the user to send a character. */
void
AC_ISP_FRAME_SEND_CHAR (uint8_t c);

/** Accept an input character. */
void
isp_frame_accept_char (uint8_t c);

/** Should be implemented by the user (isp_proto) to accept a frame.
 *
 * The provided buffer can be used by the callee. */
void
AC_ISP_FRAME_ACCEPT_FRAME (uint8_t *data, uint16_t len);

/** Send a frame, to be used by isp_proto. */
void
isp_frame_send_frame (uint8_t *data, uint16_t len);

#endif /* isp_frame_h */
