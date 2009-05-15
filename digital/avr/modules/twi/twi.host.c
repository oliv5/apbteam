/* twi.host.c */
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
#include "modules/host/mex.h"

#include <string.h>

/** This implementation should cover all usual cases, and assert in other
 * cases. */

/** Read messages are sent as request.
 * In request, first byte is address, second byte is length.
 * In response, whole payload is data. */
#define TWI_READ 0x90
/** Write messages are sent directly.
 * First byte is address, rest of payload is data. */
#define TWI_WRITE 0x91

/** TWI address. */
static uint8_t twi_address;

#if AC_TWI_SLAVE_ENABLE

/** Received data. */
static uint8_t rcpt_buf_sl[AC_TWI_SL_RECV_BUFFER_SIZE];
/** Received data size. */
static uint8_t rcpt_size_sl;
/** Whether new received data are ready. */
static uint8_t data_ready_sl;
/** Data sent on master request. */
static uint8_t send_buf_sl[AC_TWI_SL_SEND_BUFFER_SIZE];

/** Handle READ requests from master. */
static void
twi_handle_READ (void *user, mex_msg_t *msg);

/** Handle WRITE requests from master. */
static void
twi_handle_WRITE (void *user, mex_msg_t *msg);

#endif /* AC_TWI_SLAVE_ENABLE */

/** Initialise twi. */
void
twi_init (uint8_t addr)
{
    twi_address = addr;
#if AC_TWI_SLAVE_ENABLE
    data_ready_sl = 0;
    mex_node_register (TWI_READ, twi_handle_READ, NULL);
    mex_node_register (TWI_WRITE, twi_handle_WRITE, NULL);
#endif /* AC_TWI_SLAVE_ENABLE */
}

#if AC_TWI_SLAVE_ENABLE

/** Récupère dans buffer les données recues en tant qu'esclave */
uint8_t 
twi_sl_poll (uint8_t *buffer, uint8_t size)
{
    uint8_t i;
    if (data_ready_sl)
      {
	data_ready_sl = 0;
	if (size > rcpt_size_sl)
	    size = rcpt_size_sl;
	for (i = 0; i < size; i++)
	    buffer[i] = rcpt_buf_sl[i];
	return size;
      }
    else
	return 0;
}

/** Met à jour le buffer de donnée à envoyer */
void 
twi_sl_update (uint8_t *buffer, uint8_t size)
{
    while (size--)
	send_buf_sl[size] = buffer[size];
}

/** Handle READ requests from master. */
static void
twi_handle_READ (void *user, mex_msg_t *msg)
{
    u8 addr, size;
    mex_msg_pop (msg, "BB", &addr, &size);
    if (addr == twi_address)
      {
	assert (size <= AC_TWI_SL_SEND_BUFFER_SIZE);
	mex_msg_t *m = mex_msg_new (TWI_READ);
	mex_msg_push_buffer (m, send_buf_sl, size);
	mex_node_response (m);
      }
}

/** Handle WRITE requests from master. */
static void
twi_handle_WRITE (void *user, mex_msg_t *msg)
{
    u8 addr, size;
    mex_msg_pop (msg, "B", &addr);
    if (addr == twi_address)
      {
	size = mex_msg_len (msg);
	assert (size <= AC_TWI_SL_RECV_BUFFER_SIZE);
	memcpy (rcpt_buf_sl, mex_msg_pop_buffer (msg), size);
	rcpt_size_sl = size;
	data_ready_sl = 1;
      }
}

#endif /* AC_TWI_SLAVE_ENABLE */

#if AC_TWI_MASTER_ENABLE

/** Is the current transaction finished ? */
int8_t 
twi_ms_is_finished (void)
{
    return 1;
}

/** Send len bytes of data to address */
int8_t
twi_ms_send (uint8_t address, uint8_t *data, uint8_t len)
{
    mex_msg_t *m = mex_msg_new (TWI_WRITE);
    mex_msg_push (m, "B", address);
    mex_msg_push_buffer (m, data, len);
    mex_node_send (m);
    return 0;
}

/** Read len bytes at addresse en put them in data */
int8_t
twi_ms_read (uint8_t address, uint8_t *data, uint8_t len)
{
    mex_msg_t *m = mex_msg_new (TWI_READ);
    mex_msg_push (m, "BB", address, len);
    m = mex_node_request (m);
    assert (mex_msg_len (m) == len);
    memcpy (data, mex_msg_pop_buffer (m), len);
    return 0;
}

#endif /* AC_TWI_MASTER_ENABLE */

