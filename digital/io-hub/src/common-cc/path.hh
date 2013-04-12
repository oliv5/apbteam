#ifndef path_hh
#define path_hh
// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "defs.hh"

extern "C" {
#include "modules/path/astar/astar.h"
}

/** Number of possible obstacles. */
#define PATH_OBSTACLES_NB   (4+1/*cake*/)

/** Number of points for the cake */
#define PATH_CAKE_POINTS_NB 10

/** Number of points per standard obstacle. */
#define PATH_OBSTACLES_POINTS_NB 6

#define PATH_FIXED_POINTS_NB    2

/** Number of points. */
#define PATH_POINTS_NB  (PATH_FIXED_POINTS_NB + \
                         (PATH_OBSTACLES_NB * PATH_OBSTACLES_POINTS_NB) + \
                         (PATH_CAKE_POINTS_NB - PATH_OBSTACLES_POINTS_NB) + \
                         1 /*debug*/)

/** Number of nodes. */
#define PATH_NODES_NB   PATH_POINTS_NB

/** Angle between obstacles points. */
#define PATH_OBSTACLES_POINTS_ANGLE(pOINTS_NB)  ((1L << 24) / (pOINTS_NB))

/** Obstacle */
typedef struct path_obstacle_t
{
    /** Center. */
    vect_t c;
    /** Radius. */
    uint16_t r;
    /** factor */
    int factor;
} path_obstacle_t;

// Path finding class.
class Path
{
  public:
    // Initialise path
    Path();

    // Reset path computation, remove every obstacles.
    void reset(void);

    // Add an obstacle on the field
    void add_obstacle(const vect_t &c, const int r, const int factor, const int points_nb);

    // Set a moving obstacle position, radius and factor.
    void obstacle(int index, const vect_t &c, int r, int factor = 0);

    // Set path source and destination.
    void endpoints(const vect_t &src, const vect_t &dst);

    // Compute path with the given escape factor.
    void compute(int factor = 0);

    // Return a point by index
    vect_t& get_point(uint8_t index);

    // Get next point in computed path, return false if none (no path found
    // or last point given yet).
    bool get_next(vect_t &p);

    //Find all neighbors of a given node, fill the astar_neighbor structure
    uint8_t find_neighbors(uint8_t node, struct astar_neighbor_t *neighbors);

#if 0
    // Prepare score computation for the given source, with given escape factor.
    void prepare_score (const vect_t &src, int factor = 0);

    // Return score for a given destination, using a previous preparation
    // (also reuse previously given escape factor).
    int get_score (const vect_t &dst);
#endif

  private:

    /** Borders, any point outside borders is eliminated. */
    const int16_t border_xmin, border_ymin, border_xmax, border_ymax;

    /** Escape factor, 0 if none. */
    uint8_t escape_factor;

    /** List of obstacles. */
    path_obstacle_t obstacles[PATH_OBSTACLES_NB];

    uint8_t obstacles_nb;

    /** List of navigation points */
    vect_t points[PATH_POINTS_NB];

    /** Number of navigation points */
    uint8_t points_nb;

    /** List of nodes used for A*. */
    struct astar_node_t astar_nodes[PATH_NODES_NB];

    /** Which node to look at for next step. */
    uint8_t next_node;

    /** TRUE when a path has been found */
    bool path_found;
};

#endif // path_hh
