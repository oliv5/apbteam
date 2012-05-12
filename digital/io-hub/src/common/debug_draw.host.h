#ifndef debug_draw_host_h
#define debug_draw_host_h
/* debug_draw.host.h */
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
#include "defs.h"

#ifdef HOST

/** Initialise module. */
void
debug_draw_init (void);

/** Start a debug draw message, to be filled with debug draw primitives, then
 * sent. */
void
debug_draw_start (void);

/** Send prepared message. */
void
debug_draw_send (void);

/** Append a debug circle. */
void
debug_draw_circle (const vect_t *p, int16_t radius, uint8_t color);

/** Append a debug segment. */
void
debug_draw_segment (const vect_t *p1, const vect_t *p2, uint8_t color);

/** Append a debug point. */
void
debug_draw_point (const vect_t *p, uint8_t color);

#endif

#endif /* debug_draw_host_h */
