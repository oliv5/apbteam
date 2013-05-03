#ifndef path_hh
#define path_hh
// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Nicolas Schodet
// Copyright (C) 2013 Olivier Lanneluc
//
// APBTeam:
//      Web: http://apbteam.org/
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
#include "playground_2013.hh"

extern "C" {
#include "modules/path/astar/astar.h"
}

/** Static nodes index for the endpoints */
enum {
    PATH_NAVPOINT_SRC_IDX = 0,
    PATH_NAVPOINT_DST_IDX,
    PATH_RESERVED_NAVPOINTS_NB
};

/** Obstacle */
typedef struct path_obstacle_t
{
    /** Center. */
    vect_t c;
    /** Radius. */
    uint16_t r;
    /** Allow the robot to target the obstacle
     * in order to go in front of it, while stopping
     * at a given distance, before the collision happens */
    bool target_allowed;
} path_obstacle_t;

typedef uint16_t weight_t;

/** Path finding class */
class Path
{
  public:
    /** Initialise path */
    Path();
    /** Reset path computation, remove every obstacles */
    void reset(void);
    /** Set a moving obstacle position, radius and factor*/
    void obstacle(const int index, const vect_t &c, const uint16_t r, const int f = 0, const bool target = false);
    /** Set path source and destination */
    void endpoints(const vect_t &src, const vect_t &dst);
    /** Compute path with the given escape factor */
    void compute(weight_t escape = 0);
    /** Return a point vector by index */
    vect_t& get_point_vect(const int index);
    /** Return a point index */
    int get_point_index(const vect_t& point);
    /** Get next point in computed path, return false if none
       (no path found or last point given yet) */
    bool get_next(vect_t &p);
    /** Find all neighbors of a given node, fill the astar_neighbor structure */
    int find_neighbors(int node, struct astar_neighbor_t *neighbors);
    /** Prepare score computation for the given source, with given escape factor */
    void prepare_score(const vect_t &src, weight_t escape = 0);
    /** Return score for a given destination, using a previous preparation
       (also reuse previously given escape factor) */
    weight_t get_score(const vect_t &dst);

  private:
    /** Add an obstacle on the field */
    void add_obstacle(const vect_t &c, uint16_t r, const int nodes, const int nlayers, const uint16_t clearance, const bool target);

    /** Number of possible obstacles. */
    static const int PATH_OBSTACLES_NB = (4+1/*cake*/);
    /** Number of points per standard obstacle. */
    static const int PATH_OBSTACLES_NAVPOINTS_NB = 12;
#ifdef playground_2013_hh
    /** Number of points for the cake (plus 1) */
    static const int PATH_CAKE_NAVPOINTS_NB = 6;
#endif
    /** Number of navigation points layers for the cake. */
    static const int PATH_CAKE_NAVPOINTS_LAYERS = 1;
    /** Number of navigation points layers for each obstacle. */
    static const int PATH_OBSTACLES_NAVPOINTS_LAYERS = 2;
    /** Number of navigation points. */
    static const int PATH_NAVPOINTS_NB = (PATH_RESERVED_NAVPOINTS_NB +
#ifdef playground_2013_hh
                                           PATH_CAKE_NAVPOINTS_LAYERS * (PATH_CAKE_NAVPOINTS_NB - PATH_OBSTACLES_NAVPOINTS_NB) +
#endif
                                           PATH_OBSTACLES_NAVPOINTS_LAYERS * (PATH_OBSTACLES_NB * PATH_OBSTACLES_NAVPOINTS_NB));
    /** Navigation points weight precision (2^-n). */
    static const int PATH_WEIGHT_PRECISION = 4;
    /** Navigation points weight step (2^-n). */
    static const int PATH_WEIGHT_STEP = 6;

    /** Borders, any point outside borders is eliminated. */
    const uint16_t border_xmin, border_ymin, border_xmax, border_ymax;
    /** Escape factor, 0 if none. */
    weight_t escape_factor;
    /** List of obstacles. */
    path_obstacle_t obstacles[PATH_OBSTACLES_NB];
    /** Number of obstacles */
    int obstacles_nb;
    /** List of navigation points coordonates */
    vect_t navpoints[PATH_NAVPOINTS_NB];
    /** List of navigation points weights */
    weight_t navweights[PATH_NAVPOINTS_NB];
    /** Number of navigation points */
    int navpoints_nb;
    /** List of nodes used for A*. */
    struct astar_node_t astar_nodes[PATH_NAVPOINTS_NB];
    /** Which node to look at for next step. */
    uint8_t next_node;
    /** TRUE when a path has been found */
    bool path_found;
};

#endif // path_hh
