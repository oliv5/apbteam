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
#include "modules/utils/crc.h"
#include "modules/twi/twi.h"
#include "io.h"

#include "aux.h"

#ifdef HOST
# include "simu.host.h"
#endif

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
    u8 buf[AC_TWI_SLAVE_RECV_BUFFER_SIZE];
    u8 read_data;
    /* Handle incoming command. */
    while ((read_data = twi_slave_poll (buf, sizeof (buf))))
	twi_proto_callback (buf, read_data);
    /* Update status. */
    u8 status_with_crc[8];
    u8 *status = &status_with_crc[1];
    status[0] = twi_proto.seq;
    status[1] = 0
	| (control_state_is_blocked (&cs_aux[1].state) ? (1 << 3) : 0)
	| (control_state_is_finished (&cs_aux[1].state) ? (1 << 2) : 0)
	| (control_state_is_blocked (&cs_aux[0].state) ? (1 << 1) : 0)
	| (control_state_is_finished (&cs_aux[0].state) ? (1 << 0) : 0);
    status[2] = PINC;
    status[3] = v16_to_v8 (aux[0].pos, 1);
    status[4] = v16_to_v8 (aux[0].pos, 0);
    status[5] = v16_to_v8 (aux[1].pos, 1);
    status[6] = v16_to_v8 (aux[1].pos, 0);
    /* Compute CRC. */
    status_with_crc[0] = crc_compute (&status_with_crc[1],
                                      sizeof (status_with_crc) - 1);
    twi_slave_update (status_with_crc, sizeof (status_with_crc));
}

/** Handle one command. */
static void
twi_proto_callback (u8 *buf, u8 size)
{
    /* Check CRC. */
    if (crc_compute (buf + 1, size - 1) != buf[0])
	return;
    else
      {
	/* Remove the CRC of the buffer. */
	buf += 1;
	size -= 1;
      }
    /* Handle sequence number. */
    if (buf[0] == 0 || buf[0] == twi_proto.seq)
	return;
#define c(cmd, size) (cmd)
    switch (c (buf[1], 0))
      {
      case c ('z', 0):
	/* Reset. */
	cs_reset ();
	utils_reset ();
	break;
      case c ('b', 4):
	/* Move the aux0.
	 * - w: new position.
	 * - w: speed. */
	cs_aux[0].speed.max = v8_to_v16 (buf[4], buf[5]);
	aux_traj_goto_start (&aux[0], v8_to_v16 (buf[2], buf[3]));
	break;
      case c ('c', 3):
	/* Move the aux1.
	 * - w: new position.
	 * - w: speed. */
	cs_aux[1].speed.max = v8_to_v16 (buf[4], buf[5]);
	aux_traj_goto_start (&aux[1], v8_to_v16 (buf[2], buf[3]));
	break;
      case c ('B', 6):
	/* Find the zero position.
	 * - b: aux index.
	 * - w: speed.
	 * - b: use switch.
	 * - w: reset position. */
	if (buf[2] < AC_ASSERV_AUX_NB)
	  {
	    if (buf[5])
		aux_traj_find_zero_start (&aux[buf[2]],
					  v8_to_v16 (buf[3], buf[4]),
					  v8_to_v16 (buf[6], buf[7]));
	    else
		aux_traj_find_limit_start (&aux[buf[2]],
					   v8_to_v16 (buf[3], buf[4]),
					   v8_to_v16 (buf[6], buf[7]));
	  }
	else
	    buf[0] = 0;
	break;
      case c ('l', 5):
	/* Clamp.
	 * - b: aux index.
	 * - w: speed.
	 * - w: claming PWM. */
	if (buf[2] < AC_ASSERV_AUX_NB)
	     aux_traj_clamp_start (&aux[buf[2]], v8_to_v16 (buf[3], buf[4]),
				   v8_to_v16 (buf[5], buf[6]));
	else
	    buf[0] = 0;
	break;
      case c ('w', 2):
	/* Free motor.
	 * - b: aux index.
	 * - b: brake. */
	if (buf[2] < AC_ASSERV_AUX_NB)
	  {
	    output_set (&output_aux[buf[2]], 0);
	    output_brake (&output_aux[buf[2]], buf[3]);
	    control_state_set_mode (&cs_aux[buf[2]].state, CS_MODE_NONE, 0);
	  }
	else
	    buf[0] = 0;
	break;
      case c ('W', 3):
	/* Set motor output.
	 * - b: aux index.
	 * - w: value. */
	if (buf[2] < AC_ASSERV_AUX_NB)
	  {
	    output_set (&output_aux[buf[2]], v8_to_v16 (buf[3], buf[4]));
	    control_state_set_mode (&cs_aux[buf[2]].state, CS_MODE_NONE, 0);
	  }
	else
	    buf[0] = 0;
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
	  case 'Y':
	    /* Set current aux position.
	     * - b: aux index.
	     * - w: position. */
	    if (buf[0] >= AC_ASSERV_AUX_NB || size < 3)
		return 1;
	    aux[buf[0]].pos = v8_to_v16 (buf[1], buf[2]);
	    eat = 3;
	    break;
	  default:
	    return 1;
	  }
	buf += eat;
	size -= eat;
      }
    return 0;
}

