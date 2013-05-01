// path_2013.cc
// io-hub - Modular Input/Output. {{{
//
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

#include "path_2013.hh"
#ifdef HOST
#include "debug.host.hh"
#endif

Path_2013::Path_2013() :
    Path( pg_border_distance,
          pg_border_distance,
          (pg_width - pg_border_distance),
          (pg_length - pg_border_distance) )
{
    host_debug("Path_2013 constructor\n");
    obstacles = new path_obstacle_t[PATH_2013_OBSTACLES_NB];
    navpoints = new vect_t[PATH_2013_NAVPOINTS_NB];
    navweights = new weight_t[PATH_2013_NAVPOINTS_NB];
    astar_nodes = new struct astar_node_t[PATH_2013_NAVPOINTS_NB];
}

Path_2013::~Path_2013()
{
    delete[] obstacles;
    delete[] navpoints;
    delete[] navweights;
    delete[] astar_nodes;
}

void Path_2013::reset()
{
    /* Base reset */
    Path::reset();

    /* Declare the cake as an obstacle */
    add_obstacle(pg_cake_pos, pg_cake_radius, PATH_2013_CAKE_NAVPOINTS_NB, PATH_2013_CAKE_NAVPOINTS_LAYERS, 0 /* no extra clearance radius */);
}

void Path_2013::endpoints(const vect_t &src, const vect_t &dst, const bool force_move)
{
    /* Temporary code for the cake */
    if (PATH_VECT_EQUAL(&dst, &pg_cake_pos))
    {
        /* Set the endpoints */
        Path::endpoints(src,dst,true);
    }
    else
    {
        /* Set the endpoints */
        Path::endpoints(src,dst,force_move);
    }
}
