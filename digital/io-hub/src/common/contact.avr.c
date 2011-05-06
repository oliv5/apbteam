/* contact.avr.c */
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

/** Filter depth. */
#define CONTACT_JACK_FILTER_DEPTH 50

/** Contact context. */
struct contact_t
{
    /** Jack current state. */
    uint8_t jack_state;
    /** Jack filter, number of updates before state can change. */
    uint8_t jack_filter;
};

/** Global context. */
struct contact_t contact_global;
#define ctx contact_global

void
contact_init (void)
{
    /* Pull-up. */
#define CONTACT(io) IO_SET_ (io);
    CONTACT (CONTACT_COLOR);
    CONTACT (CONTACT_JACK);
    CONTACT_LIST
#undef CONTACT
    /* Default to jack out. */
    ctx.jack_state = 1;
}

void
contact_update (void)
{
    uint8_t jack_now = IO_GET (CONTACT_JACK);
    /* If state is unchanged, reset filter. */
    if (jack_now == ctx.jack_state)
	ctx.jack_filter = 0;
    /* If state changed, should be stable for CONTACT_JACK_FILTER_DEPTH before
     * it goes through the filter. */
    else if (ctx.jack_filter++ == CONTACT_JACK_FILTER_DEPTH)
      {
	ctx.jack_state = jack_now;
	ctx.jack_filter = 0;
      }
}

enum team_color_e
contact_get_color (void)
{
    return !IO_GET (CONTACT_COLOR) ? TEAM_COLOR_LEFT : TEAM_COLOR_RIGHT;
}

uint8_t
contact_get_jack (void)
{
    return ctx.jack_state;
}

uint32_t
contact_all (void)
{
    uint32_t contacts = 0, bit = 1;
#define CONTACT(io) do { \
    contacts |= (IO_GET_ (io) ? bit : 0); \
    bit <<= 1; \
} while (0);
    CONTACT (CONTACT_COLOR);
    CONTACT (CONTACT_JACK);
    CONTACT_LIST
#undef CONTACT
    return contacts;
}
