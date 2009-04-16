/* isp_proto.c */
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
#include "common.h"

#include "isp_proto.h"

#include <avr/pgmspace.h>

#define ISP_PROTO_CMD_SIGN_ON 0x01

#define ISP_PROTO_STATUS_CMD_OK 0x00
#define ISP_PROTO_STATUS_CMD_UNKNOWN 0xc9

#define ISP_PROTO_SIGNATURE "APBdev_2"

void
isp_proto_accept (uint8_t *data, uint16_t len, uint16_t buffer_size)
{
    /* Decode command. */
    switch (data [0])
      {
      case ISP_PROTO_CMD_SIGN_ON:
	/* Return programmer signature. */
	data[1] = ISP_PROTO_STATUS_CMD_OK;
	data[2] = sizeof (ISP_PROTO_SIGNATURE);
	memcpy_P (&data[3], PSTR (ISP_PROTO_SIGNATURE),
		  sizeof (ISP_PROTO_SIGNATURE));
	AC_ISP_PROTO_SEND (data, 3 + sizeof (ISP_PROTO_SIGNATURE));
	break;
      default:
	/* Unknown. */
	data[1] = ISP_PROTO_STATUS_CMD_UNKNOWN;
	AC_ISP_PROTO_SEND (data, 2);
	break;
      }
}

