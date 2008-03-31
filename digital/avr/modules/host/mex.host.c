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
#include "common.h"
#include "mex.h"
#include "socket.h"

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
};

/** Create a new message from a buffer.  Buffer will be free'd on message
 * deletion. */
static mex_msg_t *
mex_msg_new_buffer (u8 *buffer, int size);

/** Encapsulate a message and fill header. */
static void
mex_msg_encapsulate (mex_msg_t *msg, const char *fmt, ...);

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
mex_node_handle_DATE (mex_msg_t *msg);

/** Handle an incoming REQ. */
static void
mex_node_handle_REQ (mex_msg_t *msg);

/** Global context. */
static struct mex_node_t mex_node_global;

