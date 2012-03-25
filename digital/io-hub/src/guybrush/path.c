/* path.c */
/* guybrush - Eurobot 2012 AI. {{{
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
#include "defs.h"
#include "path.h"
#include "bot.h"
#include "playground_2012.h"

#include "modules/path/astar/astar.h"
#include "modules/utils/utils.h"
#include "modules/math/geometry/distance.h"

#define PATH_DEBUG 0

#if PATH_DEBUG
#include "debug.host.h"
#endif

/**
 * This year, due to the large number of obstacles, a grid like structure is
 * used for path finding on the playground.  The A* algorithm is used to find
 * path along nodes.
 *
 * TODO: this is a placeholder for the future algorithm.
 */

/** Number of possible obstacles. */
#define PATH_OBSTACLES_NB AC_PATH_OBSTACLES_NB

/** Number of fixed nodes. */
#define PATH_FIXED_NODES_NB (1)

/** Number of nodes in search graph, last two nodes are destination and source
 * nodes. */
#define PATH_NODES_NB (PATH_FIXED_NODES_NB + 2)

/** Index of destination node. */
#define PATH_DST_NODE_INDEX PATH_FIXED_NODES_NB

/** Index of source node. */
#define PATH_SRC_NODE_INDEX (PATH_DST_NODE_INDEX + 1)

/** Information on a node. */
struct path_node_t
{
    /** Whether this node can be used. */
    uint8_t usable;
};

/** Context. */
struct path_t
{
    /** List of obstacles. */
    struct path_obstacle_t obstacles[PATH_OBSTACLES_NB];
    /** Escape factor, 0 if none. */
    uint8_t escape_factor;
    /** List of nodes used for A*. */
    struct astar_node_t astar_nodes[PATH_NODES_NB];
    /** Cache of whether a node is blocked. */
    uint8_t valid[PATH_FIXED_NODES_NB];
    /** Position of end points. */
    vect_t endpoints[2];
    /** Whether the last update was a success. */
    uint8_t found;
    /** Which node to look at for next step. */
    uint8_t get;
};
static struct path_t path;

/** Static information on nodes. */
static const struct path_node_t path_nodes[PATH_FIXED_NODES_NB] = {
    /* {{{ */
      { 1 },
    /* }}} */
};

/** Compute position of a node. */
static void
path_pos (uint8_t node, vect_t *pos)
{
    assert (node < PATH_NODES_NB);
    if (node < PATH_FIXED_NODES_NB)
      {
	/* Temporary nonsense node. */
	pos->x = PG_WIDTH / 2;
	pos->y = PG_LENGTH / 2;
      }
    else
      {
	*pos = path.endpoints[node - PATH_FIXED_NODES_NB];
      }
}

/** Return 1 if the direct path between a and b nodes is blocked, also compute
 * distance. */
static uint8_t
path_blocking (uint8_t a, uint8_t b, int16_t *dp)
{
    uint8_t i;
    vect_t va;
    vect_t vb;
    uint8_t escape_factor = 0;
    uint8_t factor = 1;
    uint8_t blocking = 0;
    if (a == PATH_SRC_NODE_INDEX || b == PATH_SRC_NODE_INDEX)
	escape_factor = path.escape_factor;
    path_pos (a, &va);
    path_pos (b, &vb);
    /* Test for a blocking obstacle. */
    for (i = 0; i < PATH_OBSTACLES_NB && !blocking; i++)
      {
	if (path.obstacles[i].valid)
	  {
	    uint16_t d = distance_segment_point (&va, &vb,
						 &path.obstacles[i].c);
	    if (d < path.obstacles[i].r)
		blocking = 1;
	  }
      }
    /* Compute distance. */
    int16_t d = distance_point_point (&va, &vb);
    if (d == 0)
      {
	*dp = 0;
	return 0;
      }
    /* Handle escaping. */
    /* TODO: do not escape through a totem! */
    if (blocking)
      {
	if (escape_factor)
	  {
	    *dp = d * escape_factor;
	    return 0;
	  }
	else
	    return 1;
      }
    /* No blocking. */
    *dp = d * factor;
    return 0;
}

/** Update the cache of blocked nodes. */
static void
path_blocked_update (void)
{
    uint8_t i, j;
    for (i = 0; i < PATH_FIXED_NODES_NB; i++)
      {
	uint8_t valid = 1;
	/* First, gather information from tables. */
	if (!path_nodes[i].usable)
	    valid = 0;
	else
	  {
	    vect_t pos;
	    path_pos (i, &pos);
	    /* Then, test for obstacles. */
	    for (j = 0; j < PATH_OBSTACLES_NB; j++)
	      {
		if (path.obstacles[j].valid)
		  {
		    vect_t v = pos; vect_sub (&v, &path.obstacles[j].c);
		    uint32_t dsq = vect_dot_product (&v, &v);
		    uint32_t r = path.obstacles[j].r;
		    if (dsq <= r * r)
		      {
			valid = 0;
			break;
		      }
		  }
	      }
	  }
	/* Update cache. */
	path.valid[i] = valid;
      }
}

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
    path.escape_factor = factor;
}

void
path_obstacle (uint8_t i, vect_t c, uint16_t r, uint8_t factor,
	       uint16_t valid)
{
    assert (i < AC_PATH_OBSTACLES_NB);
    assert (factor == 0);
    path.obstacles[i].c = c;
    path.obstacles[i].r = r;
    path.obstacles[i].valid = valid;
}

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

void
path_update (void)
{
    path_blocked_update ();
    path.found = astar (path.astar_nodes, PATH_NODES_NB, PATH_DST_NODE_INDEX,
			PATH_SRC_NODE_INDEX);
    path.get = PATH_SRC_NODE_INDEX;
#if AC_PATH_REPORT
    if (path.found)
      {
	uint8_t n, len = 0;
	vect_t points[PATH_NODES_NB];
	for (n = path.get; n != PATH_DST_NODE_INDEX; n = path.astar_nodes[n].prev)
	    path_pos (n, &points[len++]);
	path_pos (n, &points[len++]);
	AC_PATH_REPORT_CALLBACK (points, len, path.obstacles,
				 PATH_OBSTACLES_NB);
      }
#endif
}

uint8_t
path_get_next (vect_t *p)
{
    if (path.found)
      {
	assert (path.get != PATH_DST_NODE_INDEX);
	uint8_t prev = path.get;
	vect_t pp;
	path_pos (prev, &pp);
	uint8_t next = path.astar_nodes[path.get].prev;
	path.get = next;
	path_pos (next, p);
	while (next != 0xff)
	  {
	    /* Try to remove useless points. */
	    uint8_t next = path.astar_nodes[path.get].prev;
	    if (next == 0xff || next == PATH_DST_NODE_INDEX)
		break;
	    vect_t np;
	    path_pos (next, &np);
	    vect_t vnp = np; vect_sub (&vnp, &pp);
	    vect_t vp = *p; vect_sub (&vp, &pp);
	    if (vect_normal_dot_product (&vp, &vnp) == 0)
	      {
		path.get = next;
		*p = np;
	      }
	    else
		break;
	  }
	return 1;
      }
    else
	return 0;
}

/** Neighbors callback for fixed nodes. */
static uint8_t
path_astar_neighbor_callback_fixed (uint8_t node,
				    struct astar_neighbor_t *neighbors)
{
    uint8_t neighbors_nb = 0;
    int16_t d;
    /* Check if direct path OK. */
    if (!path_blocking (node, PATH_SRC_NODE_INDEX, &d))
      {
	/* Add this neighbor. */
	neighbors[neighbors_nb].node = PATH_SRC_NODE_INDEX;
	neighbors[neighbors_nb].weight = d + 1;
	neighbors_nb++;
      }
#if PATH_DEBUG
    for (i = 0; i < neighbors_nb; i++)
	DPRINTF (" n %d %d\n", neighbors[i].node, neighbors[i].weight);
#endif
    return neighbors_nb;
}

/** Neighbors callback for endpoints. */
static uint8_t
path_astar_neighbor_callback_endpoints (uint8_t node,
					struct astar_neighbor_t *neighbors)
{
    uint8_t neighbors_nb = 0;
    uint8_t i;
    assert (node == PATH_DST_NODE_INDEX);
    /* Select neighbors in the fixed nodes. */
    for (i = 0; i < PATH_FIXED_NODES_NB; i++)
      {
	/* Discard blocking nodes. */
	if (!path.valid[i])
	    continue;
	/* Check if there is an obstacle along the path. */
	int16_t d;
	if (path_blocking (PATH_DST_NODE_INDEX, i, &d))
	    continue;
	/* Add this neighbor. */
	neighbors[neighbors_nb].node = i;
	neighbors[neighbors_nb].weight = d + 1;
	neighbors_nb++;
      }
    /* Check if direct path OK. */
    int16_t d;
    if (!path_blocking (PATH_DST_NODE_INDEX, PATH_SRC_NODE_INDEX, &d))
      {
	/* Add this neighbor. */
	neighbors[neighbors_nb].node = PATH_SRC_NODE_INDEX;
	neighbors[neighbors_nb].weight = d + 1;
	neighbors_nb++;
      }
#if PATH_DEBUG
    for (i = 0; i < neighbors_nb; i++)
	DPRINTF (" n %d %d\n", neighbors[i].node, neighbors[i].weight);
#endif
    return neighbors_nb;
}

uint8_t
path_astar_neighbor_callback (uint8_t node,
			      struct astar_neighbor_t *neighbors)
{
#if PATH_DEBUG
    DPRINTF ("neighbor %d\n", node);
#endif
    if (node < PATH_FIXED_NODES_NB)
	return path_astar_neighbor_callback_fixed (node, neighbors);
    else
	return path_astar_neighbor_callback_endpoints (node, neighbors);
}

uint16_t
path_astar_heuristic_callback (uint8_t node)
{
    vect_t pos;
    path_pos (node, &pos);
    return distance_point_point (&pos, &path.endpoints[0]);
}
