#ifndef path_h
#define path_h
/* path.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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

/** Initialise path finder. */
void
path_init (void);

/** Setup end points (source and destination coordinates). */
void
path_endpoints (int16_t sx, int16_t sy, int16_t dx, int16_t dy);

/** Set up an obstacle at given position with the given radius and validity
 * period. */
void
path_obstacle (uint8_t i, int16_t x, int16_t y, uint16_t r, uint16_t valid);

/** Slowly make the obstacles disappear. */
void
path_decay (void);

/** Compute shortest path. */
void
path_update (void);

/** Retrieve first path point coordinates.  Return 0 on failure. */
uint8_t
path_get_next (uint16_t *x, uint16_t *y);

#endif /* path_h */
