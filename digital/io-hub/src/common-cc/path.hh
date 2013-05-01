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

extern "C" {
#include "modules/path/astar/astar.h"
}

/** Check for vectors equality */
#define PATH_VECT_EQUAL(v1, v2) \
    ((v1)->x==(v2)->x && (v1)->y==(v2)->y)

/** Check if point is inside a circle */
#define PATH_IN_CIRCLE(pOINT, cENTER, rADIUS) \
    (distance_point_point((pOINT), (cENTER)) <= (rADIUS))

/** Path finding class */
class Path
{
  public:
    /** Navpoints weight */
    typedef uint16_t weight_t;

  private:
    /** Obstacle */
    typedef struct path_obstacle_t
    {
        /** Center. */
        vect_t c;
        /** Radius. */
        uint16_t r;
    } path_obstacle_t;
    /** Number of points per standard obstacle. */
    static const int PATH_CC_OBSTACLES_NAVPOINTS_NB = 12;
    /** Number of reserved points for the 2 endpoints  */
    static const int PATH_CC_RESERVED_NAVPOINTS_NB = 2;
    /** Number of navigation points layers for each obstacle. */
    static const int PATH_CC_OBSTACLES_NAVPOINTS_LAYERS = 2;
    /** Navigation points weight precision (2^-n). */
    static const int PATH_WEIGHT_PRECISION = 4;
    /** Navigation points weight step (2^-n). */
    static const int PATH_WEIGHT_STEP = 6;
    /** Borders, any point outside borders is eliminated. */
    const uint16_t border_xmin, border_ymin, border_xmax, border_ymax;
    /** Escape factor, 0 if none. */
    weight_t escape_factor;
    /** List of obstacles. */
    Path::path_obstacle_t* obstacles;
    /** Number of obstacles */
    int obstacles_nb;
    /** List of navigation points coordonates */
    vect_t* navpoints;
    /** List of navigation points weights */
    weight_t* navweights;
    /** Number of navigation points */
    int navpoints_nb;
    /** Max number of navigation points */
    int navpoints_nbmax;
    /** List of nodes used for A*. */
    struct astar_node_t* astar_nodes;
    /** Which node to look at for next step. */
    int next_node;
    /** TRUE when a path has been found */
    bool path_found;
    /** TRUE to allow last movement to enter an obstacle
     * This may be used to target the center of an obstacle
     * and stop closed to it */
    bool force_move;

  protected:
    /** Number of possible obstacles. */
    static const int PATH_CC_OBSTACLES_NB = 4;
    /** Number of navigation points. */
    static const int PATH_CC_NAVPOINTS_NB =
        (PATH_CC_RESERVED_NAVPOINTS_NB + PATH_CC_OBSTACLES_NAVPOINTS_LAYERS * PATH_CC_OBSTACLES_NB * PATH_CC_OBSTACLES_NAVPOINTS_NB);

  public:
    /** Constructor */
    Path(uint16_t xmin, uint16_t ymin, uint16_t xmax, uint16_t ymax, int obstacles_max, int navpoints_max);
    /** Destructor */
    virtual ~Path() = 0;
    /** Reset path computation, remove every obstacles */
    virtual void reset(void);
    /** Set a moving obstacle position, radius and factor*/
    void obstacle(int index, const vect_t &c, uint16_t r, int f = 0);
    /** Set path source and destination */
    virtual void endpoints(const vect_t &src, const vect_t &dst, const bool force_move=false);
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

  protected:
    /** Add an obstacle on the field */
    void add_obstacle(const vect_t &c, uint16_t r, const int nodes, const int nlayers, const uint16_t clearance);
};

#endif // path_hh
