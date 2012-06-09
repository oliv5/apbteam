/* debug_draw.host.c */
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
#include "debug_draw.host.h"

#include "modules/host/host.h"
#include "modules/host/mex.h"

/** Message type. */
uint8_t debug_draw_mtype;

/** Message being composed. */
mex_msg_t *debug_draw_msg;

void
debug_draw_init (void)
{
    const char *mex_instance = host_get_instance ("io-hub0", 0);
    debug_draw_mtype = mex_node_reservef ("%s:debug-draw", mex_instance);
}

void
debug_draw_start (void)
{
    assert (!debug_draw_msg);
    debug_draw_msg = mex_msg_new (debug_draw_mtype);
}

void
debug_draw_send (void)
{
    mex_node_send (debug_draw_msg);
    debug_draw_msg = 0;
}

void
debug_draw_circle (const vect_t *p, int16_t radius, uint8_t color)
{
    mex_msg_push (debug_draw_msg, "Bhhhb", 'c', p->x, p->y, radius, color);
}

void
debug_draw_segment (const vect_t *p1, const vect_t *p2, uint8_t color)
{
    mex_msg_push (debug_draw_msg, "Bhhhhb", 's', p1->x, p1->y, p2->x, p2->y,
		  color);
}

void
debug_draw_point (const vect_t *p, uint8_t color)
{
    mex_msg_push (debug_draw_msg, "Bhhb", 'p', p->x, p->y, color);
}

void
debug_draw_number (const vect_t *p, int32_t number)
{
    mex_msg_push (debug_draw_msg, "Bhhl", 'n', p->x, p->y, number);
}

