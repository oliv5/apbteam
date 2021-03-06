#ifndef isp_proto_h
#define isp_proto_h
/* isp_proto.h */
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

/** The isp_proto sub module interprets commands in the AVR068 and AVR069
 * format.  Framing is needed for AVR068 and provided by isp_frame. */

/** Requested SCK duration, in the AVR06[89] format. */
uint8_t isp_proto_sck_duration;

/** Should be implemeted by the user to send a frame. */
void
AC_ISP_PROTO_SEND (uint8_t *data, uint16_t len);

/** Accept a frame to interpret.  The provided buffer will also be used to
 * transfer frames. */
void
isp_proto_accept (uint8_t *data, uint16_t len);

#endif /* isp_proto_h */
