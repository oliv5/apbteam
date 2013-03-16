/* twi_master.c */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
#include "twi_master.h"

#if AC_AI_TWI_MASTER_ASSERV
# include "asserv.h"
#endif
#if AC_AI_TWI_MASTER_MIMOT
# include "mimot.h"
#endif
#if AC_AI_TWI_MASTER_BEACON
# include "beacon.h"
#endif

#include "modules/twi/twi.h"
#include "modules/utils/utils.h"
#include "modules/utils/crc.h"

/** Interval between retransmissions. */
#define TWI_MASTER_RETRANSMIT_INTERVAL 10

/** Maximum command payload size. */
#define TWI_MASTER_COMMAND_PAYLOAD_MAX 12

/** Maximum status payload size. */
#define TWI_MASTER_STATUS_PAYLOAD_MAX 14

/** Maximum number of pending commands. */
#define TWI_MASTER_PENDING_MAX 16

/** Position of next free command. */
#define TWI_MASTER_PENDING_TAIL \
    ((twi_master.pending_head + twi_master.pending_nb) \
     % TWI_MASTER_PENDING_MAX)

/** Pending command structure. */
struct twi_master_command_t
{
    /** Addressed slave index. */
    uint8_t slave;
    /** Command payload with space for CRC & seq. */
    uint8_t command[TWI_MASTER_COMMAND_PAYLOAD_MAX + 2];
    /** Command length including CRC & seq. */
    uint8_t length;
};

/** Global context. */
struct twi_master_t
{
    /** Retransmission counter, retransmit when it reach zero.  It is also set
     * to zero after an acknowledge so that the next pending command is
     * sent. */
    uint8_t retransmit_counter;
    /** Index of next pending command. */
    uint8_t pending_head;
    /** Number of pending commands. */
    uint8_t pending_nb;
    /** Table of pending commands.  The next command is at pending_head.
     * Next commands are stored in a circular buffer way. */
    struct twi_master_command_t pending[TWI_MASTER_PENDING_MAX];
} twi_master;

/** Callback called when a slave status has been read.
 * - status: status payload (without CRC & seq). */
typedef void (*twi_master_slave_status_cb) (uint8_t *status);

/** Information on a slave. */
struct twi_master_slave_t
{
    /** Slave address. */
    uint8_t address;
    /** Last command sequence number. */
    uint8_t seq;
    /** Size of the status payload not including CRC & seq. */
    uint8_t status_length;
    /** Status callback. */
    twi_master_slave_status_cb status_cb;
};

/** Information on all slaves. */
static struct twi_master_slave_t twi_master_slaves[] = {
#if AC_AI_TWI_MASTER_ASSERV
      { ASSERV_TWI_ADDRESS, 0, ASSERV_STATUS_LENGTH, asserv_status_cb },
#endif
#if AC_AI_TWI_MASTER_MIMOT
      { MIMOT_TWI_ADDRESS, 0, MIMOT_STATUS_LENGTH, mimot_status_cb },
#endif
#if AC_AI_TWI_MASTER_BEACON
      { BEACON_TWI_ADDRESS, 0, BEACON_STATUS_LENGTH, beacon_status_cb },
#endif
};

/** Send first pending message if available. */
static void
twi_master_send_head (void)
{
    if (twi_master.pending_nb)
      {
	struct twi_master_command_t *c =
	    &twi_master.pending[twi_master.pending_head];
	/* Send command. */
	twi_master_send (twi_master_slaves[c->slave].address, c->command,
			 c->length);
	twi_master_wait ();
	/* Reset retransmission counter. */
	twi_master.retransmit_counter = TWI_MASTER_RETRANSMIT_INTERVAL;
      }
}

/** Update slave status, return non zero on success. */
static uint8_t
twi_master_update_status (uint8_t slave, uint8_t init)
{
    uint8_t buffer[TWI_MASTER_STATUS_PAYLOAD_MAX + 2];
    /* Read status. */
    twi_master_recv (twi_master_slaves[slave].address, buffer,
		     twi_master_slaves[slave].status_length + 2);
    uint8_t ret = twi_master_wait ();
    if (ret != twi_master_slaves[slave].status_length + 2)
	return 0;
    uint8_t crc = crc_compute (buffer + 1,
			       twi_master_slaves[slave].status_length + 1);
    if (crc != buffer[0])
	return 0;
    if (init)
	twi_master_slaves[slave].seq = buffer[1];
    /* Call user callback. */
    twi_master_slaves[slave].status_cb (buffer + 2);
    /* Update pending command list. */
    if (twi_master.pending_nb)
      {
	struct twi_master_command_t *c =
	    &twi_master.pending[twi_master.pending_head];
	if (slave == c->slave && buffer[1] == c->command[1])
	  {
	    /* Successfully acknowledged. */
	    twi_master.pending_nb--;
	    twi_master.pending_head = (twi_master.pending_head + 1)
		% TWI_MASTER_PENDING_MAX;
	    /* Trigger next transmission. */
	    twi_master.retransmit_counter = 0;
	  }
      }
    /* Success. */
    return 1;
}

void
twi_master_init (void)
{
    uint8_t i;
    /* Initialise hardware. */
    twi_init (AC_IO_TWI_ADDRESS);
    /* Get first status and reset sequence number. */
    for (i = 0; i < UTILS_COUNT (twi_master_slaves); i++)
	while (!twi_master_update_status (i, 1))
	    ;
}

uint8_t
twi_master_sync (void)
{
    uint8_t i;
    /* Update all slaves status. */
    for (i = 0; i < UTILS_COUNT (twi_master_slaves); i++)
	twi_master_update_status (i, 0);
    /* If command is to be retransmitted (or transmitted for the first time
     * after a acknowledge). */
    if (twi_master.retransmit_counter == 0)
	twi_master_send_head ();
    else
	twi_master.retransmit_counter--;
    /* Synchronised if no pending command. */
    return twi_master.pending_nb == 0;
}

uint8_t *
twi_master_get_buffer (uint8_t slave)
{
    assert (slave < TWI_MASTER_DISABLED);
    assert (twi_master.pending_nb < TWI_MASTER_PENDING_MAX);
    struct twi_master_command_t *c =
	&twi_master.pending[TWI_MASTER_PENDING_TAIL];
    /* Store slave. */
    c->slave = slave;
    /* Skip CRC and sequence number. */
    return &c->command[2];
}

void
twi_master_send_buffer (uint8_t length)
{
    assert (length != 0);
    struct twi_master_command_t *c =
	&twi_master.pending[TWI_MASTER_PENDING_TAIL];
    /* Fill sequence number, compute CRC, store length. */
    c->command[1] = ++twi_master_slaves[c->slave].seq;
    c->command[0] = crc_compute (&c->command[1], length + 1);
    c->length = length + 2;
    /* Add to the list of pending command. */
    twi_master.pending_nb++;
    /* Early transmission. */
    if (twi_master.pending_nb == 1)
	twi_master_send_head ();
}

void
twi_master_send_transient_buffer (uint8_t length)
{
    assert (length != 0);
    struct twi_master_command_t *c =
	&twi_master.pending[TWI_MASTER_PENDING_TAIL];
    /* Fill sequence number, compute CRC, store length. */
    c->command[1] = ++twi_master_slaves[c->slave].seq;
    c->command[0] = crc_compute (&c->command[1], length + 1);
    c->length = length + 2;
    /* Send right now without acknowledgement. */
    twi_master_send (twi_master_slaves[c->slave].address, c->command,
		     c->length);
    twi_master_wait ();
}

