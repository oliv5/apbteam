#ifndef path_h
#define path_h
/* path.h */
/* avr.path - Path finding module. {{{
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

/** Infinite validity for an obstacle. */
#define PATH_OBSTACLE_VALID_ALWAYS 0xffff

/** Obstacle. */
struct path_obstacle_t
{
    /** Center. */
    int16_t x, y;
    /** Radius. */
    uint16_t r;
    /** Validity counter, when this is zero, the obstacle is ignored. */
    uint16_t valid;
};

/** Initialise path finder. */
void
path_init (int16_t border_xmin, int16_t border_ymin,
	   int16_t border_xmax, int16_t border_ymax);

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

#if AC_PATH_REPORT

/** Report computed path. */
void
AC_PATH_REPORT_CALLBACK (uint16_t *points, uint8_t len,
			 struct path_obstacle_t *obstacles,
			 uint8_t obstacles_nb);

#endif

#ifdef HOST

/** Output graph in Graphviz format. */
void
path_print_graph (void);

#endif

#endif /* path_h */
