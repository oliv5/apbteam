/* twi_proto.c - Implement the protocol over TWI. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "twi_proto.h"
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "modules/twi/twi.h"

struct twi_proto_t
{
    u8 seq;
};

struct twi_proto_t twi_proto;

static void
twi_proto_callback (u8 *buf, u8 size);

static u8
twi_proto_params (u8 *buf, u8 size);

/** Initialise. */
void
twi_proto_init (void)
{
    twi_init (AC_ASSERV_TWI_ADDRESS);
    twi_proto_update ();
}

/** Handle received commands and update status. */
void
twi_proto_update (void)
{
    u8 buf[AC_TWI_SL_RECV_BUFFER_SIZE];
    /* Handle incoming command. */
    while (twi_sl_poll (buf, sizeof (buf)))
	twi_proto_callback (buf, sizeof (buf));
    /* Update status. */
    u8 status[12];
    status[0] = 0;
    status[1] = twi_proto.seq;
    status[2] = v32_to_v8 (0, 3);
    status[3] = v32_to_v8 (0, 2);
    status[4] = v32_to_v8 (0, 1);
    status[5] = v32_to_v8 (0, 3);
    status[6] = v32_to_v8 (0, 2);
    status[7] = v32_to_v8 (0, 1);
    status[8] = v32_to_v8 (0, 2);
    status[9] = v32_to_v8 (0, 1);
    status[10] = v16_to_v8 (0, 1);
    status[11] = v16_to_v8 (0, 0);
    twi_sl_update (status, sizeof (status));
}

/** Handle one command. */
static void
twi_proto_callback (u8 *buf, u8 size)
{
#define c(cmd, size) (cmd)
    switch (c (buf[1], 0))
      {
      case c ('z', 0):
	/* Reset. */
	utils_reset ();
	break;
      case c ('w', 0):
	/* Set zero pwm. */
	break;
      case c ('s', 0):
	/* Stop (set zero speed). */
	break;
      case c ('l', 3):
	/* Set linear speed controlled position consign.
	 * - 3b: theta consign offset. */
	break;
      case c ('a', 2):
	/* Set linear speed controlled position consign.
	 * - w: theta consign offset. */
	break;
      case c ('f', 0):
	/* Go to the wall. */
	break;
      case c ('F', 0):
	/* Go to the dispenser. */
	break;
      case c ('b', 3):
	/* Move the arm.
	 * - w: new position.
	 * - b: speed. */
	break;
      case c ('p', x):
	/* Set parameters. */
	if (twi_proto_params (&buf[2], size - 2) != 0)
	    buf[0] = 0;
	break;
      default:
	buf[0] = 0;
	break;
      }
    /* Acknowledge. */
    twi_proto.seq = buf[0];
}

/* Handle a parameter list of change. */
static u8
twi_proto_params (u8 *buf, u8 size)
{
    u8 eat;
    while (*buf && size)
      {
	size--;
	switch (*buf++)
	  {
	  case 'X':
	    /* Set current X position.
	     * - 3b: X position. */
	    if (size < 3)
		return 1;
	    eat = 3;
	    break;
	  case 'Y':
	    /* Set current Y position.
	     * - 3b: Y position. */
	    if (size < 3)
		return 1;
	    eat = 3;
	    break;
	  case 'A':
	    /* Set current angle.
	     * - w: angle. */
	    if (size < 2)
		return 1;
	    eat = 2;
	    break;
	  case 's':
	    /* Set maximum and slow speed.
	     * - b: theta max.
	     * - b: alpha max.
	     * - b: theta slow.
	     * - b: alpha slow. */
	    if (size < 4)
		return 1;
	    eat = 4;
	    break;
	  default:
	    return 1;
	  }
	buf += eat;
	size -= eat;
      }
    return 0;
}

