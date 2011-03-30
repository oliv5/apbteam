#ifndef mex_h
#define mex_h
/* mex.h - mex library support. */
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

/** This module provides mex Node support.  In each function, any error will
 * stop the program (using assert). */

/** Define message type identifiers. */
enum mex_mtype_t
{
    MEX_MTYPE_IDLE = 0,
    MEX_MTYPE_DATE = 1,
    MEX_MTYPE_REQ = 2,
    MEX_MTYPE_RSP = 3,
    MEX_MTYPE_RES = 4,
};

/** Message structure opaque definition. */
typedef struct mex_msg_t mex_msg_t;

/** Message handler. */
typedef void (mex_handler_t) (void *user, mex_msg_t *msg);

/** Create a new message with the given type. */
mex_msg_t *
mex_msg_new (u8 mtype);

/** Release a message. */
void
mex_msg_delete (mex_msg_t *msg);

/** Add data to the message payload.
 *
 * The fmt string describes the provided data which must follow:
 *  - b: 8 bits.
 *  - h: 16 bits.
 *  - l: 32 bits.
 *
 * Uppercase is used for unsigned (but who cares?). */
void
mex_msg_push (mex_msg_t *msg, const char *fmt, ...);

/** Add data to the message payload, using a buffer. */
void
mex_msg_push_buffer (mex_msg_t *msg, const u8 *buffer, int size);

/** Get data from the message payload.
 *
 * The fmt string follows the same syntax as mex_msg_push, but pointers are
 * provided as extra arguments. */
void
mex_msg_pop (mex_msg_t *msg, const char *fmt, ...);

/** Get remaining payload from the message. */
const u8 *
mex_msg_pop_buffer (mex_msg_t *msg);

/** Get payload remaining length. */
int
mex_msg_len (mex_msg_t *msg);

/** Get message type. */
u8
mex_msg_mtype (mex_msg_t *msg);

/** Connect to the mex Hub. */
void
mex_node_connect (void);

/** Close connection. */
void
mex_node_close (void);

/** Wait forever. */
void
mex_node_wait (void);

/** Wait until a date is reached. */
void
mex_node_wait_date (u32 date);

/** Return current date. */
u32
mex_node_date (void);

/** Send a message, msg is released. */
void
mex_node_send (mex_msg_t *msg);

/** Send a request and return response, msg is released. */
mex_msg_t *
mex_node_request (mex_msg_t *msg);

/** Send a response to the currently serviced request, msg is released. */
void
mex_node_response (mex_msg_t *msg);

/** Register a handler for the given message type. */
void
mex_node_register (u8 mtype, mex_handler_t *handler, void *user);

/** Request a message type reservation. */
u8
mex_node_reserve (const char *mtype_str);

/** Request a message type reservation, using formated string. */
u8
mex_node_reservef (const char *mtype_fmt, ...)
    __attribute__ ((format (printf, 1, 2)));

#endif /* mex_h */
