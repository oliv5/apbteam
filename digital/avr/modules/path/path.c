/* path.c - Find a path between obstables. */
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
#include "common.h"

#include "modules/math/fixed/fixed.h"
#include "modules/utils/utils.h"

#include "path.h"

#ifdef HOST
# include <stdio.h>
#endif

/** Number of possible obstacles. */
#define PATH_OBSTACLES_NB AC_PATH_OBSTACLES_NB

/** Number of points per obstacle. */
#define PATH_OBSTACLES_POINTS_NB AC_PATH_OBSTACLES_POINTS_NB

/** Angle between obstacles points. */
#define PATH_OBSTACLES_POINTS_ANGLE ((1L << 24) / PATH_OBSTACLES_POINTS_NB)

/** Number of points. */
#define PATH_POINTS_NB (2 + PATH_OBSTACLES_NB * PATH_OBSTACLES_POINTS_NB)

/** Point with extra data for Dijkstra algorithm. */
struct path_point_t
{
    /** Coordinates. */
    int16_t x, y;
    /** Weight. */
    uint16_t w;
    /** Next point (preceding in the usual Dijkstra meaning). */
    uint8_t next;
    /** Is this point taken yet? */
    uint8_t taken;
};

/** Path finding context. */
struct path_t
{
    /** Borders, any point outside borders is eliminated. */
    int16_t border_xmin, border_ymin, border_xmax, border_ymax;
    /** Complex number used to position obstacles points.
     * The obstacles points are placed evenly on a circle around the center
     * point.  Complex multiplication is used to make a point rotate around
     * the center point. */
    uint32_t rot_a, rot_b;
    /** Margin to take into account approximation of the circle. */
    uint32_t margin;
    /** List of obstacles. */
    struct path_obstacle_t obstacles[PATH_OBSTACLES_NB];
    /** Escape factor, 0 if none. */
    uint8_t escape_factor;
    /** List of points.  First two points are the destination and source
     * points.  Then comes the obstacles points. */
    struct path_point_t points[PATH_POINTS_NB];
    /** Number of points. */
    uint8_t points_nb;
    /** Connection matrix, value is arc weight, 0xffff for no connection. */
    uint16_t arcs[PATH_POINTS_NB][PATH_POINTS_NB];
};

/** Global context. */
struct path_t path;

/** Initialise path finder. */
void
path_init (int16_t border_xmin, int16_t border_ymin,
	   int16_t border_xmax, int16_t border_ymax)
{
    /** Save borders. */
    path.border_xmin = border_xmin;
    path.border_ymin = border_ymin;
    path.border_xmax = border_xmax;
    path.border_ymax = border_ymax;
    /** Complex number of modulus 1 and rotation angle. */
    path.rot_a = fixed_cos_f824 (PATH_OBSTACLES_POINTS_ANGLE);
    path.rot_b = fixed_sin_f824 (PATH_OBSTACLES_POINTS_ANGLE);
    path.margin =
	fixed_div_f824 (0x11L << 20,
			fixed_cos_f824 (PATH_OBSTACLES_POINTS_ANGLE / 2));
}

/** Compute points from obstacles. */
static void
path_compute_points (void)
{
    /* Do not touch first two points, they are destination and source. */
    uint8_t i, j, p = 2;
    for (i = 0; i < PATH_OBSTACLES_NB; i++)
      {
	if (path.obstacles[i].valid)
	  {
	    /* Compute obstacle points positions around a circle. */
	    uint32_t x, y, nx;
	    x = path.obstacles[i].r; y = 0;
	    x = fixed_mul_f824 (x, path.margin);
	    for (j = 0; j < PATH_OBSTACLES_POINTS_NB; j++)
	      {
		if (j != 0)
		  {
		    nx = fixed_mul_f824 (x, path.rot_a)
			- fixed_mul_f824 (y, path.rot_b);
		    y = fixed_mul_f824 (y, path.rot_a)
			+ fixed_mul_f824 (x, path.rot_b);
		    x = nx;
		  }
		path.points[p].x = path.obstacles[i].x + (uint16_t) x;
		path.points[p].y = path.obstacles[i].y + (uint16_t) y;
		/* Check it is in playground. */
		if (path.points[p].x >= path.border_xmin
		    && path.points[p].y >= path.border_ymin
		    && path.points[p].x < path.border_xmax
		    && path.points[p].y < path.border_ymax)
		    /* Accept point. */
		    p++;
	      }
	  }
      }
    path.points_nb = p;
}

/** Compute and return distance between two points if there is no obstacle
 * between them.  Return 0xffff if there is an obstacle. */
static uint16_t
path_compute_weight (uint8_t a, uint8_t b)
{
    int32_t dx, dy;
    int16_t ab, d, m, f;
    uint8_t i;
    /* Compute distance. */
    dx = path.points[b].x - path.points[a].x;
    dy = path.points[b].y - path.points[a].y;
    ab = fixed_sqrt_ui32 (dx * dx + dy * dy);
    if (ab == 0)
	return 0;
    /* Is there an intersection with a circle. */
    for (i = 0; i < PATH_OBSTACLES_NB; i++)
      {
	if (path.obstacles[i].valid)
	  {
	    /* Compute distance to center.
	     * Use a scalar product between a segment perpendicular vector and
	     * the vector from one segment end to obstacle center. */
	    int32_t acx, acy;
	    acx = path.obstacles[i].x - path.points[a].x;
	    acy = path.obstacles[i].y - path.points[a].y;
	    d = (acx * dy - acy * dx) / ab;
	    d = UTILS_ABS (d);
	    /* If out of the circle, no problem, else, more tests. */
	    if ((uint16_t) d <= path.obstacles[i].r)
	      {
		/* If the line intersect the circle, the segment intersect the
		 * circle if it as one point on each side of an intersection. */
		m = (acx * dx + acy * dy) / ab;
		f = fixed_sqrt_ui32 (path.obstacles[i].r * path.obstacles[i].r
				     - d * d);
		if (!((m - f > 0 && m + f > 0 && m - f > ab && m + f > ab)
		      || (m - f < 0 && m + f < 0 && m - f < ab && m + f < ab)))
		  {
		    uint8_t factor = path.obstacles[i].factor;
		    if (path.escape_factor
			&& (a == 1 || b == 1)
			&& (a != 0 && b != 0)
			&& (factor == 0u || factor > path.escape_factor))
			factor = path.escape_factor;
		    return factor == 0 ? 0xffffu : factor * (uint16_t) ab;
		  }
	      }
	  }
      }
    return ab;
}

/** Fill the arc matrix. */
static void
path_compute_arcs (void)
{
    uint8_t i, j;
    for (i = 0; i < path.points_nb; i++)
      {
	path.arcs[i][i] = 0xffff;
	for (j = 0; j < i; j++)
	  {
	    path.arcs[i][j] = path_compute_weight (i, j);
	    path.arcs[j][i] = path.arcs[i][j];
	  }
      }
}

/** Apply the Dijkstra algorithm. */
static void
path_dijkstra (void)
{
    uint8_t i, u;
    uint16_t d, wmin;
    /** Initialise each points. */
    for (i = 0; i < path.points_nb; i++)
      {
	path.points[i].w = 0xffff;
	path.points[i].next = 0xff;
	path.points[i].taken = 0;
      }
    /** Start with the destination point. */
    u = 0;
    path.points[u].w = 0;
    path.points[u].taken = 1;
    do
      {
	/* Relax every connected points. */
	for (i = 0; i < path.points_nb; i++)
	  {
	    if (path.arcs[i][u] != 0xffff)
	      {
		d = path.points[u].w + path.arcs[i][u];
		if (d < path.points[i].w)
		  {
		    path.points[i].w = d;
		    path.points[i].next = u;
		  }
	      }
	  }
	/* Find the next best point. */
	wmin = 0xffff;
	u = 0xff;
	for (i = 0; i < path.points_nb; i++)
	  {
	    if (!path.points[i].taken && path.points[i].w < wmin)
	      {
		u = i;
		wmin = path.points[u].w;
	      }
	  }
	if (u != 0xff)
	    path.points[u].taken = 1;
	/* Until the source point is found, or no solution. */
      } while (u != 1 && u != 0xff);
}

/** Setup end points (source and destination coordinates). */
void
path_endpoints (int16_t sx, int16_t sy, int16_t dx, int16_t dy)
{
    path.points[0].x = dx;
    path.points[0].y = dy;
    path.points[1].x = sx;
    path.points[1].y = sy;
}

/** Try to escape from inside an obstacle.  Bigger factor will shorten path
 * followed inside the obstacle.  Valid until the next update. */
void
path_escape (uint8_t factor)
{
    path.escape_factor = factor;
}

/** Set up an obstacle at given position with the given radius, factor and
 * validity period. */
void
path_obstacle (uint8_t i, int16_t x, int16_t y, uint16_t r, uint8_t factor,
	       uint16_t valid)
{
    assert (i < AC_PATH_OBSTACLES_NB);
    path.obstacles[i].x = x;
    path.obstacles[i].y = y;
    path.obstacles[i].r = r;
    path.obstacles[i].factor = factor;
    path.obstacles[i].valid = valid;
}

/** Slowly make the obstacles disappear. */
void
path_decay (void)
{
    uint8_t i;
    for (i = 0; i < PATH_OBSTACLES_NB; i++)
      {
	if (path.obstacles[i].valid
	    && path.obstacles[i].valid != PATH_OBSTACLE_VALID_ALWAYS)
	    path.obstacles[i].valid--;
      }
}

/** Compute shortest path. */
void
path_update (void)
{
    path_compute_points ();
    path_compute_arcs ();
    path.escape_factor = 0;
    path_dijkstra ();
#if AC_PATH_REPORT
    uint8_t len, i;
    uint16_t points[PATH_POINTS_NB * 2];
    len = 0;
    for (i = 1; i != 0xff; i = path.points[i].next)
      {
	points[len++] = path.points[i].x;
	points[len++] = path.points[i].y;
      }
    AC_PATH_REPORT_CALLBACK (points, len, path.obstacles, PATH_OBSTACLES_NB);
#endif /* AC_PATH_REPORT */
}

/** Retrieve first path point coordinates.  Return 0 on failure. */
uint8_t
path_get_next (uint16_t *x, uint16_t *y)
{
    uint8_t next;
    next = path.points[1].next;
    if (next == 0xff)
	return 0;
    else
      {
	*x = path.points[next].x;
	*y = path.points[next].y;
	return 1;
      }
}

#ifdef HOST

#include <stdio.h>

/** Output graph in Graphviz format. */
void
path_print_graph (void)
{
    int i, j;
    printf ("graph map {\n overlap = scale\n sep = 0.5\n");
    for (i = 0; i < path.points_nb; i++)
      {
	printf (" %d [ shape = Mrecord, label = \"{%d|{%d|%d}}\", "
		"pos = \"%d,%d\" ]\n", i, i, path.points[i].x,
		path.points[i].y, path.points[i].x, path.points[i].y);
	for (j = 0; j < i; j++)
	  {
	    if (path.arcs[i][j] != 0xffff)
	      {
		printf ("  %d -- %d [ label = \"%d\" ]\n", i, j,
			path.arcs[i][j]);
	      }
	  }
      }
    printf ("}\n// Path:\n");
    for (i = 1; i != 0xff; i = path.points[i].next)
	printf ("// %d, %d\n", path.points[i].x, path.points[i].y);
}

#endif
