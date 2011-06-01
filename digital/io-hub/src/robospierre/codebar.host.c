/* codebar.host.c */
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
#include "codebar.h"
#include "defs.h"

#include "modules/host/host.h"
#include "modules/host/mex.h"

uint8_t codebar_front, codebar_back;

static void
codebar_handle (void *user, mex_msg_t *msg)
{
    mex_msg_pop (msg, "BB", &codebar_front, &codebar_back);
}

void
codebar_init (void)
{
    const char *mex_instance = host_get_instance ("io-hub0", 0);
    uint8_t mtype = mex_node_reservef ("%s:codebar", mex_instance);
    mex_node_register (mtype, codebar_handle, 0);
}

uint8_t
codebar_get (uint8_t direction)
{
    if (direction == DIRECTION_FORWARD)
	return codebar_front;
    else
	return codebar_back;
}

