// path.cc
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

#include "path.hh"
#include "bot.hh"
#include "robot.hh"
#include "playground.hh"
#ifdef HOST
#include "debug.host.hh"
#endif

extern "C" {
#include "modules/math/fixed/fixed.h"
#include "modules/math/geometry/distance.h"
#include "modules/utils/utils.h"
}

/** Enable path finding debug code */
#define PATH_DEBUG      1

/** Define path finding plot ID for the python simu framework */
#define PATH_PLOT_ID    2

/** Angle between obstacles points. */
#define PATH_ANGLE_824(pOINTS_NB) \
    ((1L << 24) / (pOINTS_NB))

/** Check for vectors equality */
#define PATH_VECT_EQUAL(v1, v2) \
    ((v1)->x==(v2)->x && (v1)->y==(v2)->y)

/** Check if point is inside a circle */
#define PATH_IN_CIRCLE(pOINT, cENTER, rADIUS) \
    (distance_point_point((pOINT), (cENTER)) <= (rADIUS))

/** Static nodes index for the endpoints */
enum {
    PATH_NAVPOINT_SRC_IDX = 0,
    PATH_NAVPOINT_DST_IDX
};

Path::Path() :
    border_xmin(pg_border_distance),
    border_ymin(pg_border_distance),
    border_xmax(pg_width - pg_border_distance),
    border_ymax(pg_length - pg_border_distance),
    escape_factor(0),
    obstacles_nb(0),
    navpoints_nb(0),
    next_node(0)
{
    host_debug("Path constructor\n");
}

void Path::reset()
{
    vect_t nul = {0,0};

    /* Reset everything */
    host_debug("Path reset\n");
    obstacles_nb = 0;
    navpoints_nb = PATH_RESERVED_NAVPOINTS_NB;
    navpoints[PATH_NAVPOINT_SRC_IDX] = nul;
    navpoints[PATH_NAVPOINT_DST_IDX] = nul;
    next_node = 0;
    escape_factor = 0;

#ifdef playground_2013_hh
    /* Declare the cake as an obstacle */
    add_obstacle(pg_cake_pos, pg_cake_radius, PATH_CAKE_NAVPOINTS_NB, PATH_CAKE_NAVPOINTS_LAYERS, 0 /* no extra clearance radius */);
#endif
}

void Path::add_obstacle(const vect_t &c, uint16_t r, const int nodes, const int nlayers, const uint16_t clearance)
{
    uint32_t rot_a, rot_b, nr;
    uint32_t x, y, nx;
    int npt, layer;

    host_debug("Add obstacle c=(%u;%u) r=%u nodes=%u layers=%u\n",c.x, c.y, r, nodes, nlayers);

    /* Enlarge the obstacle radius by the robot size and clearance area width */
    r += BOT_SIZE_RADIUS /*BOT_SIZE_SIDE*/ + clearance;

    /* Store obstacle */
    //assert(obstacles_nb < PATH_OBSTACLES_NB);
    obstacles[obstacles_nb].c = c;
    obstacles[obstacles_nb].r = r;
    obstacles_nb++;

    /* Complex number of modulus 1 and rotation angle */
    nr = PATH_ANGLE_824(nodes);
    rot_a = fixed_cos_f824(nr);
    rot_b = fixed_sin_f824(nr);

    /* Extend the points radius to allow the robot to go */
    /* from one to another without collision with the */
    /* obstacle circle. New radius is r / cos(angle/2) */
    nr = PATH_ANGLE_824(nodes*2);
    x = fixed_div_f824(r, fixed_cos_f824(nr)) + 1 /* margin */;
    y = 0;

    /* Add a number of sets of navigation points with different weights */
    //for(weight=PATH_OBSTACLES_NAVPOINTS_LAYERS; weight>0; weight--)
    for(layer=nlayers-1; layer>=0; layer--)
    {
        /* Compute obstacle points positions around a circle */
        for (npt=0; npt<nodes; npt++)
        {
            /* Compute the point absolute position */
            navpoints[navpoints_nb].x = c.x + (vect_value_t)x;
            navpoints[navpoints_nb].y = c.y + (vect_value_t)y;

            /* Check it is in playground */
            if (navpoints[navpoints_nb].x >= border_xmin
             && navpoints[navpoints_nb].y >= border_ymin
             && navpoints[navpoints_nb].x <= border_xmax
             && navpoints[navpoints_nb].y <= border_ymax)
            {
                /* Accept point */
                navweights[navpoints_nb] = (1 << PATH_WEIGHT_PRECISION) + (layer * PATH_WEIGHT_STEP);
                host_debug("Add point %u (%u;%u) w=%u\n",
                        navpoints_nb, navpoints[navpoints_nb].x, navpoints[navpoints_nb].y, navweights[navpoints_nb]);
                navpoints_nb++;
            }

            /* Complex multiply with A = cos(angle) + i sin(angle) */
            nx = fixed_mul_f824(x, rot_a) - fixed_mul_f824(y, rot_b);
            y =  fixed_mul_f824(y, rot_a) + fixed_mul_f824(x, rot_b);
            x = nx;
        }

        /* Enlarge the circle */
        x += BOT_SIZE_RADIUS;
    }

#ifdef HOST
    /* Plot obstacle points */
    robot->hardware.simu_report.pos( &navpoints[PATH_RESERVED_NAVPOINTS_NB], navpoints_nb-PATH_RESERVED_NAVPOINTS_NB, PATH_PLOT_ID);
#endif
}

int Path::find_neighbors(int cur_point, struct astar_neighbor_t *neighbors)
{
    int neighbors_nb = 0;

    /* Parse all nodes */
    for(int i=0; i<navpoints_nb; i++)
    {
        /* Except the current one */
        if (i!=cur_point)
        {
            /* Get segment length */
            weight_t weight = navweights[i] * (weight_t)distance_point_point(&navpoints[cur_point], &navpoints[i]);
            weight >>= PATH_WEIGHT_PRECISION;
            host_debug("- Node %u (%u;%u) w=%u (%u) ", i, navpoints[i].x, navpoints[i].y, weight, navweights[i]);

            /* Check every obstacle */
            for(int j=0; j<obstacles_nb; j++)
            {
                /* Check for intersection with obstacle */
                uint16_t d = distance_segment_point(&navpoints[cur_point], &navpoints[i], &obstacles[j].c);
                if (d < obstacles[j].r)
                {
                    /* Collision while forcing the last move to the destination */
                    if (force_move && cur_point==PATH_NAVPOINT_DST_IDX &&
                        PATH_VECT_EQUAL(&navpoints[cur_point], &obstacles[j].c))
                    {
                        /* Skip this obstacle */
                        host_debug("in collision with c=(%u;%u) r=%u allowed ",
                            obstacles[j].c.x, obstacles[j].c.y, obstacles[j].r);
                        continue;
                    }
                    /* Collision while trying to escape the source point */
                    /* if and only if the source point is in this obstacle */
                    else if (i==PATH_NAVPOINT_SRC_IDX &&
                             PATH_IN_CIRCLE(&navpoints[i], &obstacles[j].c, obstacles[j].r))
                    {
                        /* Allow this navigation point with an extra cost */
                        weight *= escape_factor;
                    }
                    /* Collision during a standard move */
                    else
                    {
                        /* Disable this navigation point */
                        weight = 0;
                    }
                    host_debug("in collision with c=(%u;%u) r=%u w=%u ",
                        obstacles[j].c.x, obstacles[j].c.y, obstacles[j].r, weight);
                    break; /* Stop checking for obstacles with this node */
                }
            }

            /* Add this navigation point in the neighbor list when valid */
            if (weight)
            {
                host_debug("=> validated w=%u\n", weight);
                neighbors[neighbors_nb].node = i;
                neighbors[neighbors_nb].weight = weight;
                neighbors_nb++;
            }
            else
            {
                host_debug("=> skipped\n");
            }
        }
    }

#if PATH_DEBUG
    host_debug("\tFound %u neighbors: ", neighbors_nb);
    for(int i=0;i<neighbors_nb;i++)
        host_debug("%u (%u)  ", neighbors[i].node, neighbors[i].weight);
    host_debug("\n");
#endif

    return neighbors_nb;
}

void Path::compute(weight_t escape)
{
    host_debug("** Path compute(start) escape=%u\n", escape);

    /* Store the escape factor */
    escape_factor = escape;

    /* Call the A* algorithm */
    path_found = (bool)astar(astar_nodes, PATH_NAVPOINTS_NB, PATH_NAVPOINT_DST_IDX, PATH_NAVPOINT_SRC_IDX);
    if (path_found)
    {
        /* Store next node to go to */
        next_node = astar_nodes[0].prev;

#if PATH_DEBUG
        /* Log and display the path found */
        vect_t path[PATH_NAVPOINTS_NB];
        int node = PATH_NAVPOINT_SRC_IDX;
        int path_nb = 0;

        host_debug(">> Path found: ");
        while(node!=PATH_NAVPOINT_DST_IDX)
        {
            host_debug("%u (%u), ", node, navweights[node]);
            path[path_nb++] = navpoints[node];
            node = astar_nodes[node].prev;
        }
        path[path_nb++] = navpoints[node];
        robot->hardware.simu_report.path(path, path_nb);
        host_debug("%u\n", node);
    }
#endif

    host_debug("** Path compute(end) found=%u escape=%u\n", path_found, escape);
}

void Path::obstacle(int index, const vect_t &c, uint16_t r, int f)
{
    add_obstacle(c, r, PATH_OBSTACLES_NAVPOINTS_NB, PATH_OBSTACLES_NAVPOINTS_LAYERS, Obstacles::clearance_mm + 100/* extra clearance radius */);
}

void Path::endpoints(const vect_t &src, const vect_t &dst, const bool force_move)
{
    /* Store endpoints location */
    host_debug("Set path endpoints src=(%u;%u) dst=(%u;%u)\n",
            src.x, src.y, dst.x, dst.y);
    navpoints[PATH_NAVPOINT_SRC_IDX] = src;
    navpoints[PATH_NAVPOINT_DST_IDX] = dst;

    /* Init endpoints weights */
    navweights[PATH_NAVPOINT_SRC_IDX] = (1<<PATH_WEIGHT_PRECISION);
    navweights[PATH_NAVPOINT_DST_IDX] = (1<<PATH_WEIGHT_PRECISION);

    /* Set force_move to allow the path to end inside an obstacle */
    /* This is useful to target the center of an obstacle and stop */
    /* in front of it before the collision happens */
    this->force_move = force_move;

#ifdef playground_2013_hh
    /* Temporary code for the cake */
    if (PATH_VECT_EQUAL(&dst, &pg_cake_pos))
    {
        this->force_move = true;
    }
#endif
}

bool Path::get_next(vect_t &p)
{
    if (path_found)
    {
        p = navpoints[next_node];
        next_node = astar_nodes[next_node].prev;
    }
    return path_found;
}

vect_t& Path::get_point_vect(const int index)
{
    //assert(index<navpoints_nb);
    return navpoints[index];
}

int Path::get_point_index(const vect_t& point)
{
    for(int i=0; i<navpoints_nb; i++)
    {
        if (navpoints[i].x==point.x && navpoints[i].y==point.y)
            return i;
    }
    return -1;
}

void Path::prepare_score(const vect_t &src, weight_t escape)
{
    host_debug("Path prepare score from src=(%u;%u) escape=%u\n", src.x, src.y, escape);
    escape_factor = escape;
    astar_dijkstra_prepare(astar_nodes, PATH_NAVPOINTS_NB, get_point_index(src), PATH_NAVPOINT_DST_IDX);
}

weight_t Path::get_score(const vect_t &dst)
{
    uint16_t score = astar_dijkstra_finish(astar_nodes, PATH_NAVPOINTS_NB, get_point_index(dst));
    host_debug("Path get score=%u for dst=(%u;%u)\n", score, dst.x, dst.y);
    return score;
}

extern "C" uint8_t
AC_ASTAR_NEIGHBOR_CALLBACK (uint8_t node, struct astar_neighbor_t *neighbors)
{
#if PATH_DEBUG
    vect_t point_v = robot->path.get_point_vect(node);
    host_debug("AC_ASTAR_NEIGHBOR_CALLBACK node=%u (%u;%u)\n", node, point_v.x, point_v.y);
#endif
    return robot->path.find_neighbors(node, neighbors);
}

extern "C" uint16_t
AC_ASTAR_HEURISTIC_CALLBACK (uint8_t node)
{
    vect_t point_a = robot->path.get_point_vect(node);
    vect_t point_b = robot->path.get_point_vect(PATH_NAVPOINT_SRC_IDX);
    int16_t dist = distance_point_point(&point_a, &point_b);
    host_debug("Heuristic node=%u dist=%u\n", node, dist);
    return dist;
}
