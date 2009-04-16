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

/* isp - ISP module. */
/** Size of isp_frame buffer. */
#define AC_ISP_FRAME_BUFFER_SIZE 275
/** Should be implemented by the user to send a character. */
#define AC_ISP_FRAME_SEND_CHAR uart0_putc
/** Should be implemented by the user (isp_proto) to accept a frame. */
#define AC_ISP_FRAME_ACCEPT_FRAME isp_proto_accept
/** Should be implemeted by the user to send a frame. */
#define AC_ISP_PROTO_SEND isp_frame_send_frame

#endif /* avrconfig_h */
