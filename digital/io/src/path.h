#ifndef path_h
#define path_h
/* path.h - Path finding for Eurobot 2010. */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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

/** This implement a interface similar to the path module, but adapted for the
 * special grid of Eurobot 2010.  See real path modules for interface comments. */

/** Infinite validity for an obstacle. */
#define PATH_OBSTACLE_VALID_ALWAYS 0xffff

/** Obstacle. */
struct path_obstacle_t
{
    /** Center. */
    vect_t c;
    /** Radius. */
    uint16_t r;
    /** Validity counter, when this is zero, the obstacle is ignored. */
    uint16_t valid;
};

void
path_init (int16_t border_xmin, int16_t border_ymin,
	   int16_t border_xmax, int16_t border_ymax);

void
path_endpoints (vect_t s, vect_t d);

void
path_escape (uint8_t factor);

void
path_obstacle (uint8_t i, vect_t c, uint16_t r, uint8_t factor,
	       uint16_t valid);

void
path_decay (void);

void
path_update (void);

uint8_t
path_get_next (vect_t *p);

#if AC_PATH_REPORT

/** Report computed path. */
void
AC_PATH_REPORT_CALLBACK (vect_t *points, uint8_t len,
			 struct path_obstacle_t *obstacles,
			 uint8_t obstacles_nb);

#endif

#endif /* path_h */
