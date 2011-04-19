/* mex.host.c - mex library support. */
/* avr.host - Host fonctions modules. {{{
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
#define _GNU_SOURCE
#include "common.h"
#include "mex.h"
#include "host.h"
#include "socket.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/** Message header allocated size. */
#define MEX_MSG_NEW_HEADER_SIZE 3

/** Message payload allocated size. */
#define MEX_MSG_NEW_PAYLOAD_SIZE 128

/** Message buffer allocated size. */
#define MEX_MSG_NEW_BUFFER_SIZE \
    (MEX_MSG_NEW_HEADER_SIZE + MEX_MSG_NEW_PAYLOAD_SIZE)

/** Default Hub address. */
#define MEX_DEFAULT_ADDRESS "localhost", "2442"

/** Message structure.
 *
 * Message buffer is allocated at message creation.  Space is reserved at
 * buffer start to allow encapsulation.
 *
 * A message can only be sent if header_tail == payload_head. */
struct mex_msg_t
{
    /** Message type. */
    u8 mtype;
    /** Allocated buffer. */
    u8 *buffer_head, *buffer_tail;
    /** Header portion of the message. */
    u8 *header_head, *header_tail;
    /** Payload portion of the message. */
    u8 *payload_head, *payload_tail;
};

/** Global context structure. */
struct mex_node_t
{
    /** Connection with the Hub. */
    int socket;
    /** Current date. */
    u32 date;
    /** Current sequence number, used for Hub synchronisation. */
    u8 seq;
    /** Currently handled request, or -1 if none. */
    int req;
    /** Table of registered handlers. */
    mex_handler_t *handlers[256];
    /** Table of registered handlers user parameters. */
    void *handlers_user[256];
};

/** Create a new message from a buffer.  Buffer will be free'd on message
 * deletion. */
static mex_msg_t *
mex_msg_new_buffer (u8 *buf, int size);

/** Encapsulate a message and fill header. */
static void
mex_msg_encapsulate (mex_msg_t *msg, u8 mtype, const char *fmt, ...);

/** Decapsulate a message, the current payload becomes the full message. */
static void
mex_msg_decapsulate (mex_msg_t *msg);

/** Receive one message. */
static mex_msg_t *
mex_node_recv (void);

/** Call the right handler for the given message. */
static void
mex_node_dispatch (mex_msg_t *msg);

/** Handle an incoming DATE. */
static void
mex_node_handle_DATE (void *user, mex_msg_t *msg);

/** Handle an incoming REQ. */
static void
mex_node_handle_REQ (void *user, mex_msg_t *msg);


/** Global context. */
static struct mex_node_t mex_node_global = { .socket = -1, .req = -1 };


/** Create a new message with the given type. */
mex_msg_t *
mex_msg_new (u8 mtype)
{
    mex_msg_t *msg = malloc (sizeof (mex_msg_t));
    assert (msg);
    msg->mtype = mtype;
    /* Allocate buffer and fill header. */
    u8 *buf = malloc (MEX_MSG_NEW_BUFFER_SIZE);
    assert (buf);
    msg->buffer_head = buf;
    msg->buffer_tail = buf + MEX_MSG_NEW_BUFFER_SIZE;
    u8 *header = buf + MEX_MSG_NEW_HEADER_SIZE - 1;
    msg->header_head = header;
    msg->header_tail = header + 1;
    *header = mtype;
    msg->payload_head = msg->header_tail;
    msg->payload_tail = msg->payload_head;
    return msg;
}

/** Create a new message from a buffer.  Buffer will be free'd on message
 * deletion. */
static mex_msg_t *
mex_msg_new_buffer (u8 *buf, int size)
{
    assert (size >= 1);
    mex_msg_t *msg = malloc (sizeof (mex_msg_t));
    assert (msg);
    /* Decode header. */
    msg->mtype = buf[0];
    /* Position pointers. */
    msg->buffer_head = buf;
    msg->buffer_tail = buf + size;
    msg->header_head = buf;
    msg->header_tail = buf + 1;
    msg->payload_head = buf + 1;
    msg->payload_tail = buf + size;
    return msg;
}

/** Release a message. */
void
mex_msg_delete (mex_msg_t *msg)
{
    free (msg->buffer_head);
    free (msg);
}

/** Add data to the message payload.
 *
 * The fmt string describes the provided data which must follow:
 *  - b: 8 bits.
 *  - h: 16 bits.
 *  - l: 32 bits.
 *
 * Uppercase is used for unsigned (but who cares?). */
void
mex_msg_push (mex_msg_t *msg, const char *fmt, ...)
{
    const char *p;
    va_list a;
    va_start (a, fmt);
    for (p = fmt; *p; p++)
      {
	/* Due to promotion, all types are int (on platforms with 32 bit and
	 * more integers). */
	unsigned int v = va_arg (a, unsigned int);
	switch (*p)
	  {
	  case 'l':
	  case 'L':
	    assert (msg->payload_tail + 4 <= msg->buffer_tail);
	    *msg->payload_tail++ = v >> 24;
	    *msg->payload_tail++ = v >> 16;
	    /* no break */
	  case 'h':
	  case 'H':
	    assert (msg->payload_tail + 2 <= msg->buffer_tail);
	    *msg->payload_tail++ = v >> 8;
	    /* no break */
	  case 'b':
	  case 'B':
	    assert (msg->payload_tail + 1 <= msg->buffer_tail);
	    *msg->payload_tail++ = v >> 0;
	    break;
	  default:
	    assert (0);
	  }
      }
    va_end (a);
}

/** Add data to the message payload, using a buffer. */
void
mex_msg_push_buffer (mex_msg_t *msg, const u8 *buf, int size)
{
    assert (size > 0 && msg->payload_tail + size <= msg->buffer_tail);
    memcpy (msg->payload_tail, buf, size);
    msg->payload_tail += size;
}

/** Get data from the message payload.
 *
 * The fmt string follows the same syntax as mex_msg_push, but pointers are
 * provided as extra arguments. */
void
mex_msg_pop (mex_msg_t *msg, const char *fmt, ...)
{
    const char *p;
    va_list a;
    va_start (a, fmt);
    u32 v;
    u32 *p32;
    u16 *p16;
    u8 *p8;
    for (p = fmt; *p; p++)
      {
	const u8 *h = msg->payload_head;
	switch (*p)
	  {
	  case 'l':
	  case 'L':
	    assert (h + 4 <= msg->payload_tail);
	    v = (h[0] << 24) | (h[1] << 16) | (h[2] << 8) | (h[3] << 0);
	    p32 = va_arg (a, u32 *);
	    *p32 = v;
	    msg->payload_head += 4;
	    break;
	  case 'h':
	  case 'H':
	    assert (h + 2 <= msg->payload_tail);
	    v = (h[0] << 8) | (h[1] << 0);
	    p16 = va_arg (a, u16 *);
	    *p16 = v;
	    msg->payload_head += 2;
	    break;
	  case 'b':
	  case 'B':
	    assert (h + 1 <= msg->payload_tail);
	    v = (h[0] << 0);
	    p8 = va_arg (a, u8 *);
	    *p8 = v;
	    msg->payload_head += 1;
	    break;
	  default:
	    assert (0);
	  }
      }
    va_end (a);
}

/** Get remaining payload from the message. */
const u8 *
mex_msg_pop_buffer (mex_msg_t *msg)
{
    return msg->payload_head;
}

/** Get payload remaining length. */
int
mex_msg_len (mex_msg_t *msg)
{
    return msg->payload_tail - msg->payload_head;
}

/** Get message type. */
u8
mex_msg_mtype (mex_msg_t *msg)
{
    return msg->mtype;
}

/** Encapsulate a message and fill header. */
static void
mex_msg_encapsulate (mex_msg_t *msg, u8 mtype, const char *fmt, ...)
{
    assert (msg->header_tail == msg->payload_head);
    msg->payload_head = msg->header_head;
    /* Count new header size. */
    const char *p;
    int hsize = 1;
    for (p = fmt; *p; p++)
      {
	switch (*p)
	  {
	  case 'l':
	  case 'L':
	    hsize += 4;
	    break;
	  case 'h':
	  case 'H':
	    hsize += 2;
	    break;
	  case 'b':
	  case 'B':
	    hsize += 1;
	    break;
	  }
      }
    msg->header_head -= hsize;
    msg->header_tail = msg->header_head;
    assert (hsize >= 0 && msg->header_head >= msg->buffer_head);
    /* Message type. */
    *msg->header_tail++ = mtype;
    /* Fill header. */
    va_list a;
    va_start (a, fmt);
    for (p = fmt; *p; p++)
      {
	/* Due to promotion, all types are int (on platforms with 32 bit and
	 * more integers). */
	unsigned int v = va_arg (a, unsigned int);
	switch (*p)
	  {
	  case 'l':
	  case 'L':
	    *msg->header_tail++ = v >> 24;
	    *msg->header_tail++ = v >> 16;
	    /* no break */
	  case 'h':
	  case 'H':
	    *msg->header_tail++ = v >> 8;
	    /* no break */
	  case 'b':
	  case 'B':
	    *msg->header_tail++ = v >> 0;
	    break;
	  default:
	    assert (0);
	  }
      }
    assert (msg->header_tail == msg->payload_head);
}

/** Decapsulate a message, the current payload becomes the full message. */
static void
mex_msg_decapsulate (mex_msg_t *msg)
{
    assert (msg->payload_head + 1 <= msg->payload_tail);
    msg->header_head = msg->payload_head;
    msg->header_tail = msg->payload_head = msg->header_head + 1;
    msg->mtype = msg->header_head[0];
}

/** Connect to the mex Hub. */
void
mex_node_connect (void)
{
    assert (mex_node_global.socket == -1);
    /* Try to reuse an already opened connection. */
    mex_node_global.socket = host_fetch_integer ("mex_node_socket");
    if (mex_node_global.socket == -1)
      {
	/* Connect. */
	mex_node_global.socket = socket_client (MEX_DEFAULT_ADDRESS);
	host_register_integer ("mex_node_socket", mex_node_global.socket);
      }
    else
      {
	/* Request synchronisation. */
	mex_msg_t *m = mex_msg_new (MEX_MTYPE_DATE);
	mex_node_send (m);
      }
    /* Setup default handlers. */
    mex_node_register (MEX_MTYPE_DATE, mex_node_handle_DATE, NULL);
    mex_node_register (MEX_MTYPE_REQ, mex_node_handle_REQ, NULL);
    /* Synchronise with the Hub. */
    int sync = 0;
    while (!sync)
      {
	mex_msg_t *m = mex_node_recv ();
	if (m->mtype == MEX_MTYPE_DATE)
	    sync = 1;
	mex_node_dispatch (m);
	mex_msg_delete (m);
      }
}

/** Query connection status, returns 0 if not connected. */
int
mex_node_connected (void)
{
    return mex_node_global.socket != -1;
}

/** Close connection. */
void
mex_node_close (void)
{
    assert (mex_node_global.socket != -1);
    close (mex_node_global.socket);
    mex_node_global.socket = -1;
    mex_node_global.req = -1;
}

/** Wait forever. */
void
mex_node_wait (void)
{
    mex_node_wait_date ((u32) -1);
}

/** Wait until a date is reached. */
void
mex_node_wait_date (u32 date)
{
    while (date == (u32) -1 || mex_node_global.date != date)
      {
	/* Signal IDLE mode. */
	mex_msg_t *idle = mex_msg_new (MEX_MTYPE_IDLE);
	if (date != (u32) -1)
	    mex_msg_push (idle, "L", date);
	mex_node_send (idle);
	/* Receive and dispatch message. */
	mex_msg_t *m = mex_node_recv ();
	mex_node_dispatch (m);
	mex_msg_delete (m);
      }
}

/** Return current date. */
u32
mex_node_date (void)
{
    return mex_node_global.date;
}

/** Send a message, msg is released. */
void
mex_node_send (mex_msg_t *msg)
{
    assert (mex_node_global.socket != -1);
    assert (msg->header_tail == msg->payload_head);
    int size = msg->payload_tail - msg->header_head;
    int r;
    u8 header[3];
    header[0] = size >> 8;
    header[1] = size >> 0;
    header[2] = mex_node_global.seq;
    r = write (mex_node_global.socket, header, sizeof (header));
    if (r == -1)
	assert_perror (errno);
    assert (r == sizeof (header));
    r = write (mex_node_global.socket, msg->header_head, size);
    if (r == -1)
	assert_perror (errno);
    assert (r == size);
    mex_msg_delete (msg);
}

/** Send a request and return response, msg is released. */
mex_msg_t *
mex_node_request (mex_msg_t *msg)
{
    /* Send request. */
    mex_msg_encapsulate (msg, MEX_MTYPE_REQ, "B", 0);
    mex_node_send (msg);
    /* Wait for response. */
    mex_msg_t *rsp;
    rsp = mex_node_recv ();
    while (rsp->mtype != MEX_MTYPE_RSP)
      {
	mex_node_dispatch (rsp);
	mex_msg_delete (rsp);
	rsp = mex_node_recv ();
      }
    /* Eat unused reqid. */
    u8 reqid;
    mex_msg_pop (rsp, "B", &reqid);
    mex_msg_decapsulate (rsp);
    return rsp;
}

/** Send a response to the currently serviced request, msg is released. */
void
mex_node_response (mex_msg_t *msg)
{
    assert (mex_node_global.req != -1);
    mex_msg_encapsulate (msg, MEX_MTYPE_RSP, "B", mex_node_global.req);
    mex_node_send (msg);
    mex_node_global.req = -1;
}

/** Register a handler for the given message type. */
void
mex_node_register (u8 mtype, mex_handler_t *handler, void *user)
{
    assert (mex_node_global.handlers[mtype] == NULL);
    mex_node_global.handlers[mtype] = handler;
    mex_node_global.handlers_user[mtype] = user;
}

/** Request a message type reservation. */
u8
mex_node_reserve (const char *mtype_str)
{
    /* Send request. */
    mex_msg_t *m = mex_msg_new (MEX_MTYPE_RES);
    mex_msg_push_buffer (m, mtype_str, strlen (mtype_str));
    mex_node_send (m);
    /* Wait for response. */
    mex_msg_t *rsp;
    rsp = mex_node_recv ();
    while (rsp->mtype != MEX_MTYPE_RES)
      {
	mex_node_dispatch (rsp);
	mex_msg_delete (rsp);
	rsp = mex_node_recv ();
      }
    /* Return allocated message type. */
    u8 mtype;
    mex_msg_pop (rsp, "B", &mtype);
    return mtype;
}

/** Request a message type reservation, using formated string. */
u8
mex_node_reservef (const char *mtype_fmt, ...)
{
    va_list ap;
    char mtype_str[MEX_MSG_NEW_PAYLOAD_SIZE + 1];
    va_start (ap, mtype_fmt);
    int r = vsnprintf (mtype_str, sizeof (mtype_str), mtype_fmt, ap);
    assert (r < (int) sizeof (mtype_str));
    va_end (ap);
    return mex_node_reserve (mtype_str);
}

/** Receive one message. */
static mex_msg_t *
mex_node_recv (void)
{
    int r;
    assert (mex_node_global.socket != -1);
    /* Read header. */
    u8 header[3];
    r = read (mex_node_global.socket, header, sizeof (header));
    if (r == -1)
	assert_perror (errno);
    else if (r == 0)
      {
	mex_node_close ();
	exit (0);
      }
    /* Decode header. */
    assert (r == sizeof (header));
    int size = (header[0] << 8) | header[1];
    mex_node_global.seq = header[2];
    /* Read whole message. */
    u8 *buf = malloc (size);
    assert (buf);
    r = read (mex_node_global.socket, buf, size);
    if (r == -1)
	assert_perror (errno);
    assert (r == size);
    mex_msg_t *msg = mex_msg_new_buffer (buf, size);
    return msg;
}

/** Call the right handler for the given message. */
static void
mex_node_dispatch (mex_msg_t *msg)
{
    if (mex_node_global.handlers[msg->mtype])
      {
	mex_node_global.handlers[msg->mtype]
	    (mex_node_global.handlers_user[msg->mtype], msg);
      }
}

/** Handle an incoming DATE. */
static void
mex_node_handle_DATE (void *user, mex_msg_t *msg)
{
    mex_msg_pop (msg, "L", &mex_node_global.date);
}

/** Handle an incoming REQ. */
static void
mex_node_handle_REQ (void *user, mex_msg_t *msg)
{
    u8 req;
    mex_msg_pop (msg, "B", &req);
    mex_node_global.req = req;
    mex_msg_decapsulate (msg);
    mex_node_dispatch (msg);
    mex_node_global.req = -1;
}

