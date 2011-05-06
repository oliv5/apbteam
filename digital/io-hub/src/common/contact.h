#ifndef contact_h
#define contact_h
/* contact.h */
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
#include "simu.host.h"
#include "defs.h"
#include "io.h"

#include "contact_defs.h"

/** To get a state of a generic contact, use: IO_GET (CONTACT_EXAMPLE). */

/** Initialize contact module. */
void
contact_init (void);

/** Update contact module.  This handle debouncing. */
void
contact_update (void);

/** Get color switch state, unfiltered. */
enum team_color_e
contact_get_color (void);

/** Get filtered jack state. */
uint8_t
contact_get_jack (void);

/** Return state of all contact, unfiltered. */
uint32_t
contact_all (void);

#endif /* contact_h */
