/* output.host.c */
/* io-hub - Modular Input/Output. {{{
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
#include "output.h"

#include "modules/host/host.h"
#include "modules/host/mex.h"

/** Output context. */
struct output_t
{
    /** Previous sent state. */
    uint32_t previous;
    /** Message type. */
    uint8_t mtype;
};

/** Global context. */
struct output_t output_global;
#define ctx output_global

void
output_host_init (void)
{
    const char *mex_instance = host_get_instance ("io-hub0", 0);
    ctx.mtype = mex_node_reservef ("%s:output", mex_instance);
    ctx.previous = (uint32_t) -1;
}

void
output_host_update (void)
{
    uint32_t new = 0, bit = 1;
    /* Collect all output states. */
#define OUTPUT(io, init) do { \
    if (IO_PORT_ (io) & IO_BV_ (io)) \
	new |= bit; \
    bit <<= 1; \
} while (0); // <- do not copy this code unless you know why!
    OUTPUT_LIST
#undef OUTPUT
    /* If changed, sent it. */
    if (ctx.previous != new)
      {
	mex_msg_t *m = mex_msg_new (ctx.mtype);
	mex_msg_push (m, "L", new);
	mex_node_send (m);
	ctx.previous = new;
      }
}

