/* path.c */
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
#include "path.h"

/** Context. */
struct path_t
{
    /** Position of end points. */
    vect_t endpoints[2];
    /** Whether the last update was a success. */
    uint8_t found;
};
static struct path_t path;

void
path_init (void)
{
}

void
path_endpoints (vect_t s, vect_t d)
{
    path.endpoints[0] = d;
    path.endpoints[1] = s;
}

void
path_escape (uint8_t factor)
{
}

void
path_obstacle (uint8_t i, vect_t c, uint16_t r, uint8_t factor,
	       uint16_t valid)
{
}

void
path_decay (void)
{
}

void
path_update (void)
{
    path.found = 1;
}

uint8_t
path_get_next (vect_t *p)
{
    if (path.found)
      {
	*p = path.endpoints[0];
	return 1;
      }
    else
	return 0;
}

