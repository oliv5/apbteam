/* twi.host.c - Implementation for host system. */
/* avr.twi - TWI AVR module. {{{
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
#include "twi.h"

#include "modules/utils/utils.h"
#include "modules/host/mex.h"
#include <string.h>

/** Read messages are sent as request.
 * In request, first byte is address, second byte is length.
 * In response, whole payload is data. */
#define TWI_READ 0x90
/** Write messages are sent directly.
 * First byte is address, rest of payload is data. */
#define TWI_WRITE 0x91

/** Module context. */
struct twi_host_t
{
    /** Slave address. */
    uint8_t address;
#if AC_TWI_SLAVE_ENABLE
    /** Slave transmission buffer. */
    uint8_t slave_send_buffer[AC_TWI_SLAVE_SEND_BUFFER_SIZE];
    /** Slave transmission buffer current size. */
    uint8_t slave_send_buffer_size;
#endif /* AC_TWI_SLAVE_ENABLE */
#if AC_TWI_MASTER_ENABLE
    /** Current master status. */
    uint8_t master_current_status;
#endif /* AC_TWI_MASTER_ENABLE */
};

/** Global context. */
static struct twi_host_t twi_host_global;
#define ctx twi_host_global

#if AC_TWI_SLAVE_ENABLE

/** Handle READ requests from master. */
static void
twi_handle_READ (void *user, mex_msg_t *msg);

/** Handle WRITE requests from master. */
static void
twi_handle_WRITE (void *user, mex_msg_t *msg);

#endif /* AC_TWI_SLAVE_ENABLE */

void
twi_init (uint8_t addr)
{
    assert ((addr & 1) == 0);
    ctx.address = addr;
#if AC_TWI_SLAVE_ENABLE
    ctx.slave_send_buffer_size = 1;
    ctx.slave_send_buffer[0] = 0;
    mex_node_register (TWI_READ, twi_handle_READ, NULL);
    mex_node_register (TWI_WRITE, twi_handle_WRITE, NULL);
#endif /* AC_TWI_SLAVE_ENABLE */
#if AC_TWI_MASTER_ENABLE
    ctx.master_current_status = TWI_MASTER_ERROR;
#endif /* AC_TWI_MASTER_ENABLE */
}

void
twi_uninit (void)
{
    ctx.address = 0xff;
}

#if AC_TWI_SLAVE_ENABLE

/** Update buffer to be sent to master if requested. */
void
twi_slave_update (const uint8_t *buffer, uint8_t size)
{
    assert (size && size <= AC_TWI_SLAVE_SEND_BUFFER_SIZE);
    memcpy (ctx.slave_send_buffer, buffer, size);
    ctx.slave_send_buffer_size = size;
}

/** Handle READ requests from master. */
static void
twi_handle_READ (void *user, mex_msg_t *msg)
{
    uint8_t addr, size;
    mex_msg_pop (msg, "BB", &addr, &size);
    if (addr == ctx.address)
      {
	assert (size <= AC_TWI_SLAVE_SEND_BUFFER_SIZE);
	mex_msg_t *m = mex_msg_new (TWI_READ);
	mex_msg_push_buffer (m, ctx.slave_send_buffer,
			     UTILS_MIN (size, ctx.slave_send_buffer_size));
	mex_node_response (m);
      }
}

/** Handle WRITE requests from master. */
static void
twi_handle_WRITE (void *user, mex_msg_t *msg)
{
    uint8_t addr, size;
    mex_msg_pop (msg, "B", &addr);
    if (addr == ctx.address)
      {
	size = mex_msg_len (msg);
	AC_TWI_SLAVE_RECV (mex_msg_pop_buffer (msg),
			   UTILS_MIN (size, AC_TWI_SLAVE_RECV_BUFFER_SIZE));
      }
}

#endif /* AC_TWI_SLAVE_ENABLE */

#if AC_TWI_MASTER_ENABLE

void
twi_master_send (uint8_t addr, const uint8_t *buffer, uint8_t size)
{
    /* Send message. */
    mex_msg_t *m = mex_msg_new (TWI_WRITE);
    mex_msg_push (m, "B", addr);
    mex_msg_push_buffer (m, buffer, size);
    mex_node_send (m);
    /* Update status, there is no background task. */
    ctx.master_current_status = size;
    /* If defined, call master done callback. */
#ifdef AC_TWI_MASTER_DONE
    AC_TWI_MASTER_DONE ();
#endif
}

void
twi_master_recv (uint8_t addr, uint8_t *buffer, uint8_t size)
{
    /* Send request and wait for response. */
    mex_msg_t *m = mex_msg_new (TWI_READ);
    mex_msg_push (m, "BB", addr, size);
    m = mex_node_request (m);
    int recv = mex_msg_len (m);
    assert (recv <= size);
    memcpy (buffer, mex_msg_pop_buffer (m), recv);
    /* Update status, there is no background task. */
    ctx.master_current_status = recv;
    /* If defined, call master done callback. */
#ifdef AC_TWI_MASTER_DONE
    AC_TWI_MASTER_DONE ();
#endif
}

uint8_t
twi_master_status (void)
{
    return ctx.master_current_status;
}

uint8_t
twi_master_wait (void)
{
    /* No background task, nothing to wait. */
    return ctx.master_current_status;
}

#endif /* AC_TWI_MASTER_ENABLE */

#if AC_TWI_NO_INTERRUPT

void
twi_update (void)
{
    /* Nothing to do. */
}

#endif /* AC_TWI_NO_INTERRUPT */

