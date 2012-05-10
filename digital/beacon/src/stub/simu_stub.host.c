/* simu_stub.c */
/* lol - Laser Opponent Location finding system. {{{
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
#include "common.h"

#include "modules/host/host.h"
#include "modules/host/mex.h"
#include "modules/twi/twi.h"
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "modules/utils/crc.h"

/** This is a stub only used in simulation to bypass all beacon computations
 * and directly provide a position via TWI. */

struct position_t
{
    uint16_t x, y;
};

struct position_t simu_positions[2];

uint8_t simu_seq;

uint8_t simu_recv_jack;
uint8_t simu_recv_robot_nb;
struct position_t simu_recv_robot_position;

static void
simu_twi_proto_poll (void)
{
    /* Do nothing interresting with the received value, but check it
     * anyway. */
    uint8_t recv_buf[AC_TWI_SLAVE_RECV_BUFFER_SIZE];
    uint8_t recv_size;
    while ((recv_size = twi_slave_poll (recv_buf, sizeof (recv_buf))))
      {
	if (crc_compute (recv_buf + 1, recv_size - 1) != recv_buf[0])
	    continue;
	if (recv_buf[1] == simu_seq)
	    continue;
	if (recv_size != 8)
	    continue;
	simu_seq = recv_buf[1];
	simu_recv_jack = recv_buf[2];
	simu_recv_robot_nb = recv_buf[3];
	simu_recv_robot_position.x = v8_to_v16 (recv_buf[4], recv_buf[5]);
	simu_recv_robot_position.y = v8_to_v16 (recv_buf[6], recv_buf[7]);
      }
}

static void
simu_twi_proto_update (void)
{
    /* Update TWI slave buffer after each position update. */
    uint8_t status[4 + 5 * UTILS_COUNT (simu_positions)];
    uint8_t i, index = 1;
    status[index++] = 0;
    status[index++] = 0;
    status[index++] = simu_seq;
    for (i = 0; i < UTILS_COUNT (simu_positions); i++)
      {
	status[index++] = v16_to_v8 (simu_positions[i].x, 1);
	status[index++] = v16_to_v8 (simu_positions[i].x, 0);
	status[index++] = v16_to_v8 (simu_positions[i].y, 1);
	status[index++] = v16_to_v8 (simu_positions[i].y, 0);
	status[index++] = simu_positions[i].x == (uint16_t) -1 ? 0 : 100;
      }
    status[0] = crc_compute (&status[1], sizeof (status) - 1);
    twi_slave_update (status, sizeof (status));
}

static void
simu_positions_handle (void *user, mex_msg_t *msg)
{
    uint8_t i;
    struct position_t pos;
    mex_msg_pop (msg, "BHH", &i, &pos.x, &pos.y);
    simu_positions[i] = pos;
    simu_twi_proto_update ();
}

static void
simu_init (void)
{
    const char *mex_instance;
    mex_node_connect ();
    mex_instance = host_get_instance ("beacon0", 0);
    uint8_t mtype = mex_node_reservef ("%s:position", mex_instance);
    mex_node_register (mtype, simu_positions_handle, 0);
    uint8_t i;
    for (i = 0; i < UTILS_COUNT (simu_positions); i++)
      {
	simu_positions[i].x = (uint16_t) -1;
	simu_positions[i].y = (uint16_t) -1;
      }
}

int
main (int argc, char ** argv)
{
    avr_init (argc, argv);
    simu_init ();
    twi_init (AC_BEACON_TWI_ADDRESS);
    simu_twi_proto_update ();
    while (1)
      {
	simu_twi_proto_poll ();
	mex_node_wait_date (mex_node_date () + 1);
      }
    return 0;
}

