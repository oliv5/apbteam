/* path.c - Path finding for Eurobot 2010. */
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
#include "common.h"
#include "defs.h"
#include "path.h"

#include "food.h"

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
 * The grid is composed of 13 columns of 5 node each.  They are numbered by
 * column.  See eurobot/2010/table for drawings of the grid.  Even columns are
 * aligned with playing elements, while odd columns are connecting nodes
 * placed on the middle of segments connecting playing elements.  Therefore,
 * odd columns have a offset of 125 mm, and that is the reason why code should
 * handle odd and even columns differently.
 *
 * All those tricks are used to reduce the number of nodes.
 */

/** Number of possible obstacles. */
#define PATH_OBSTACLES_NB AC_PATH_OBSTACLES_NB

/** Number of nodes in a column. */
#define PATH_COLUMN_NODES_NB 5

/** Number of columns. */
#define PATH_COLUMNS_NB 13

/** Number of nodes in the grid. */
#define PATH_GRID_NODES_NB (PATH_COLUMNS_NB * PATH_COLUMN_NODES_NB)

/** Number of nodes in search graph, last two nodes are destination and source
 * nodes. */
#define PATH_NODES_NB (PATH_GRID_NODES_NB + 2)

/** Index of destination node. */
#define PATH_DST_NODE_INDEX PATH_GRID_NODES_NB

/** Index of source node. */
#define PATH_SRC_NODE_INDEX (PATH_DST_NODE_INDEX + 1)

/** Information on a node. */
struct path_node_t
{
    /** Whether this node can be used. */
    uint8_t usable;
    /** If this node can carry a corn, this is the index of the carried corn
     * in the food table, else, 0xff. */
    uint8_t carry_corn;
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
    uint8_t valid[PATH_GRID_NODES_NB];
    /** Position of end points. */
    vect_t endpoints[2];
    /** Whether the last update was a success. */
    uint8_t found;
    /** Which node to look at for next step. */
    uint8_t get;
};
static struct path_t path;

/** Static information on nodes. */
static const struct path_node_t path_nodes[PATH_GRID_NODES_NB] = {
    /* {{{ */
      { 1,    0 }, /*  0 column 0. */
      { 1, 0xff }, /*  1 */
      { 1,    2 }, /*  2 */
      { 1, 0xff }, /*  3 */
      { 1,    4 }, /*  4 */
      { 1, 0xff }, /*  5 column 1. */
      { 1, 0xff }, /*  6 */
      { 1, 0xff }, /*  7 */
      { 1, 0xff }, /*  8 */
      { 1, 0xff }, /*  9 */
      { 1, 0xff }, /* 10 column 2. */
      { 1,    5 }, /* 11 */
      { 1, 0xff }, /* 12 */
      { 1,    7 }, /* 13 */
      { 1, 0xff }, /* 14 */
      { 1, 0xff }, /* 15 column 3. */
      { 1, 0xff }, /* 16 */
      { 1, 0xff }, /* 17 */
      { 1, 0xff }, /* 18 */
      { 1, 0xff }, /* 19 */
      { 0, 0xff }, /* 20 column 4. */
      { 1, 0xff }, /* 21 */
      { 1,   10 }, /* 22 */
      { 1, 0xff }, /* 23 */
      { 1,   12 }, /* 24 */
      { 1, 0xff }, /* 25 column 5. */
      { 1, 0xff }, /* 26 */
      { 1, 0xff }, /* 27 */
      { 1, 0xff }, /* 28 */
      { 1, 0xff }, /* 29 */
      { 0, 0xff }, /* 30 column 6. */
      { 1, 0xff }, /* 31 */
      { 1, 0xff }, /* 32 */
      { 1,   29 }, /* 33 */
      { 1, 0xff }, /* 34 */
      { 1, 0xff }, /* 35 column 7. */
      { 1, 0xff }, /* 36 */
      { 1, 0xff }, /* 37 */
      { 1, 0xff }, /* 38 */
      { 1, 0xff }, /* 39 */
      { 0, 0xff }, /* 40 column 8. */
      { 1, 0xff }, /* 41 */
      { 1,   24 }, /* 42 */
      { 1, 0xff }, /* 43 */
      { 1,   26 }, /* 44 */
      { 1, 0xff }, /* 45 column 9. */
      { 1, 0xff }, /* 46 */
      { 1, 0xff }, /* 47 */
      { 1, 0xff }, /* 48 */
      { 1, 0xff }, /* 49 */
      { 1, 0xff }, /* 50 column 10. */
      { 1,   19 }, /* 51 */
      { 1, 0xff }, /* 52 */
      { 1,   21 }, /* 53 */
      { 1, 0xff }, /* 54 */
      { 1, 0xff }, /* 55 column 11. */
      { 1, 0xff }, /* 56 */
      { 1, 0xff }, /* 57 */
      { 1, 0xff }, /* 58 */
      { 1, 0xff }, /* 59 */
      { 1,   14 }, /* 60 column 12. */
      { 1, 0xff }, /* 61 */
      { 1,   16 }, /* 62 */
      { 1, 0xff }, /* 63 */
      { 1,   18 }, /* 64 */
    /* }}} */
};

/** Compute position of a node. */
static void
path_pos (uint8_t node, vect_t *pos)
{
    assert (node < PATH_NODES_NB);
    if (node < PATH_GRID_NODES_NB)
      {
	uint8_t col = node / PATH_COLUMN_NODES_NB;
	uint8_t line = node - col * PATH_COLUMN_NODES_NB;
	pos->x = 150 + col * 450 / 2;
	pos->y = 128 + PATH_COLUMN_NODES_NB * 250
	    - (col % 2 ? 250 / 2 : 0)
	    - line * 250;
      }
    else
      {
	*pos = path.endpoints[node - PATH_GRID_NODES_NB];
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
    if (a == PATH_SRC_NODE_INDEX || b == PATH_SRC_NODE_INDEX)
	escape_factor = path.escape_factor;
    path_pos (a, &va);
    path_pos (b, &vb);
    /* Test for a blocking obstacle. */
    for (i = 0; i < PATH_OBSTACLES_NB; i++)
      {
	if (path.obstacles[i].valid)
	  {
	    uint16_t d = distance_segment_point (&va, &vb,
						 &path.obstacles[i].c);
	    if (d < path.obstacles[i].r)
	      {
		if (escape_factor)
		  {
		    int16_t d = distance_point_point (&va, &vb);
		    *dp = d * escape_factor;
		    return 0;
		  }
		else
		    return 1;
	      }
	  }
      }
    /* Test for a blocking food. */
    int16_t d = distance_point_point (&va, &vb);
    if (d == 0)
      {
	*dp = 0;
	return 0;
      }
    else if (food_blocking_path (va, vb, d))
      {
	if (escape_factor)
	  {
	    *dp = d * escape_factor;
	    return 0;
	  }
	else
	    return 1;
      }
    else
      {
	*dp = d;
	return 0;
      }
}

/** Update the cache of blocked nodes. */
static void
path_blocked_update (void)
{
    uint8_t i, j;
    for (i = 0; i < PATH_GRID_NODES_NB; i++)
      {
	uint8_t valid = 1;
	/* First, gather information from tables. */
	if (!path_nodes[i].usable
	    || food_blocking (path_nodes[i].carry_corn))
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
path_init (int16_t border_xmin, int16_t border_ymin,
	   int16_t border_xmax, int16_t border_ymax)
{
    /* Border are ignored as only the grid is used, nothing else to do. */
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
	/* TODO: remove useless nodes. */
	assert (path.get != PATH_DST_NODE_INDEX);
	uint8_t next = path.astar_nodes[path.get].prev;
	path.get = next;
	path_pos (next, p);
	return 1;
      }
    else
	return 0;
}

/** Neighbors callback for nodes in grid. */
static uint8_t
path_astar_neighbor_callback_grid (uint8_t node,
				   struct astar_neighbor_t *neighbors)
{
    uint8_t neighbors_nb = 0;
    uint8_t i;
    /* Add neighbors in all 6 directions. */
    static const struct {
	/** Column offset of this neighbor. */
	int8_t column_offset;
	/** Line offset of this neighbor. */
	int8_t line_offset;
	/** Extra line offset for odd columns. */
	int8_t odd_line_offset;
	/** Distance to this neighbor. */
	uint16_t weight;
    } star_n[] = {
	  { 0, -1, 0, 250 }, /* N */
	  { -1, -1, 1, 514 / 2 }, /* NW */
	  { -1, 0, 1, 514 / 2 }, /* SW */
	  { 0, 1, 0, 250 }, /* S */
	  { 1, 0, 1, 514 / 2 }, /* SE */
	  { 1, -1, 1, 514 / 2 }, /* NE */
    };
    uint8_t col = node / PATH_COLUMN_NODES_NB;
    uint8_t line = node - col * PATH_COLUMN_NODES_NB;
    uint8_t odd = col % 2;
    for (i = 0; i < UTILS_COUNT (star_n); i++)
      {
	int8_t new_col = col + star_n[i].column_offset;
	int8_t new_line = line + star_n[i].line_offset
	    + (odd ? star_n[i].odd_line_offset : 0);
	int8_t new_node = new_col * PATH_COLUMN_NODES_NB + new_line;
	if (new_col >= 0 && new_col < PATH_COLUMNS_NB
	    && new_line >= 0 && new_line < PATH_COLUMN_NODES_NB
	    && path.valid[new_node])
	  {
	    neighbors[neighbors_nb].node = new_node;
	    neighbors[neighbors_nb].weight = star_n[i].weight + 1;
	    neighbors_nb++;
	  }
      }
    /* Check if direct path OK. */
    int16_t d;
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
    /* Select neighbors in the grid. */
    for (i = 0; i < PATH_GRID_NODES_NB; i++)
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
    if (node < PATH_GRID_NODES_NB)
	return path_astar_neighbor_callback_grid (node, neighbors);
    else
	return path_astar_neighbor_callback_endpoints (node, neighbors);
}

uint16_t
path_astar_heuristic_callback (uint8_t node)
{
    /* TODO: a better and faster heuristic can be found, considering that
     * movement is only allowed on the grid. */
    vect_t pos;
    path_pos (node, &pos);
    return distance_point_point (&pos, &path.endpoints[0]);
}
