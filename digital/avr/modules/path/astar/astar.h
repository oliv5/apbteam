#ifndef astar_h
#define astar_h
/* astar.h */
/* avr.path.astar - A* path finding module. {{{
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

/** Used instead of score for a unvisited node. */
#define ASTAR_NODE_SCORE_UNVISITED ((uint16_t) 0xffff)
/** Used instead of score for a node in the closed set. */
#define ASTAR_NODE_SCORE_CLOSED ((uint16_t) 0xfffe)
/** Test whether a node is in the open set. */
#define ASTAR_NODE_SCORE_OPEN(score) ((score) < ASTAR_NODE_SCORE_CLOSED)

/** Node information.  Instantiated by the client in an array to store
 * information on every nodes in the graph. */
struct astar_node_t
{
    /** Previous node in path. */
    uint8_t prev;
    /** Score of this node (often named g(x) in A* literature),
     * ASTAR_NODE_SCORE_UNVISITED if unvisited, or ASTAR_NODE_SCORE_CLOSED if
     * in the closed set.  Score is the sum of all weights from the initial
     * node to this node.  Every nodes with a score are in the open set. */
    uint16_t score;
    /** Heuristic weight from this node to the goal (often named h(x) in A*
     * literature).  It will be computed only once, so this is initialised to
     * 0 and set using a user provided function. */
    uint16_t heuristic;
};

/** Information filled by the user neighbor callback. */
struct astar_neighbor_t
{
    /** Neighbor node to inspect. */
    uint8_t node;
    /** Weight of the arc between the requested node and this neighbor
     * node. */
    uint16_t weight;
};

/** Neighbor callback.
 * - node: currently inspected node.
 * - neighbors: array to be filled by this callback.
 * - returns: number of neighbors.
 * This callback is to be provided by the user.  It should fill the neighbor
 * array with all neighbors reachable from the requested node. */
uint8_t
AC_ASTAR_NEIGHBOR_CALLBACK (uint8_t node, struct astar_neighbor_t *neighbors);

/** Heuristic callback.
 * - node: considered node.
 * - returns: heuristic weight from the considered node to the goal.
 * This callback is to be provided by the user.  This should be an optimistic
 * heuristic or A* may not found the best path. */
uint16_t
AC_ASTAR_HEURISTIC_CALLBACK (uint8_t node);

/** A* algorithm.
 * - nodes: array of all nodes.
 * - nodes_nb: number of nodes.
 * - initial: initial node index.
 * - goal: goal node index.
 * - returns: non zero if a path is found.
 * If a path is found, user can extract the found path by following the prev
 * member, starting from the goal node.
 *
 * It may be more convenient to invert initial and goal so that the path can
 * be followed from the starting point to the destination point instead of the
 * other way round. */
uint8_t
astar (struct astar_node_t *nodes, uint8_t nodes_nb, uint8_t initial,
       uint8_t goal);

#endif /* astar_h */
