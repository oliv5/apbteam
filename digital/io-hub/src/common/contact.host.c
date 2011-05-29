/* contact.host.c */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "contact.h"

#include "modules/host/host.h"
#include "modules/host/mex.h"

/** Contact context. */
struct contact_t
{
    /** All contact state. */
    uint32_t all;
    /** Jack current state. */
    uint8_t jack_state;
    /** Color current state. */
    enum team_color_e color_state;
};

/** Global context. */
struct contact_t contact_global;
#define ctx contact_global

static void
contact_handle (void *user, mex_msg_t *msg)
{
    uint32_t contacts;
    mex_msg_pop (msg, "L", &contacts);
    ctx.all = contacts;
    ctx.color_state = (contacts & 1) ? TEAM_COLOR_LEFT : TEAM_COLOR_RIGHT;
    ctx.jack_state = (contacts & 2) ? 1 : 0;
    contacts >>= 2;
#define CONTACT(io) do { \
    if (contacts & 1) \
	IO_PIN_ (io) |= IO_BV_ (io); \
    else \
	IO_PIN_ (io) &= ~IO_BV_ (io); \
    contacts >>= 1; \
} while (0);
    CONTACT_LIST
#undef CONTACT
}

void
contact_init (void)
{
    const char *mex_instance = host_get_instance ("io-hub0", 0);
    uint8_t mtype = mex_node_reservef ("%s:contact", mex_instance);
    mex_node_register (mtype, contact_handle, 0);
    /* Default to jack out. */
    ctx.jack_state = 1;
}

void
contact_update (void)
{
    /* Nothing to do. */
}

enum team_color_e
contact_get_color (void)
{
    return ctx.color_state;
}

uint8_t
contact_get_jack (void)
{
    return ctx.jack_state;
}

uint32_t
contact_all (void)
{
    return ctx.all;
}
