// path.cc
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

#include "path.hh"
#include "bot.hh"
#include "robot.hh"
#include "playground.hh"
#include "playground_2013.hh"

extern "C" {
#include "modules/math/fixed/fixed.h"
#include "modules/math/geometry/distance.h"
#include "modules/utils/utils.h"
}

#define PATH_DEBUG      1

#ifdef HOST
#include "../../digital/ai/src/common/debug.host.h"
#endif

#define PATH_PLOT_ID    2

enum {
    PATH_POINT_SRC_IDX = 0,
    PATH_POINT_DST_IDX
};

Path::Path() :
    border_xmin(pg_border_distance),
    border_ymin(pg_border_distance),
    border_xmax(pg_width - pg_border_distance),
    border_ymax(pg_length - pg_border_distance),
    escape_factor(0),
    obstacles_nb(0),
    points_nb(0),
    next_node(0)
{
    DPRINTF("Path constructor\n");
}

void Path::reset()
{
    vect_t nul = {0,0};

    /* Reset everything */
    DPRINTF( "Path reset\n");
    obstacles_nb = 0;
    points_nb = PATH_RESERVED_POINTS_NB;
    points[PATH_POINT_SRC_IDX] = nul;
    points[PATH_POINT_DST_IDX] = nul;
    next_node = 0;
    escape_factor = 0;

    /* Declare the cake as an obstacle */
    add_obstacle(pg_cake_pos, pg_cake_radius + BOT_SIZE_SIDE * 4 / 3, 0, PATH_CAKE_POINTS_NB);
}

void Path::add_obstacle(const vect_t &c, const int r, const int factor, const int num)
{
    uint32_t rot_a, rot_b, fact_r;
    uint32_t x, y, nx;
    int j;

    DPRINTF("Add obstacle c=(%u;%u) r=%u factor=%u points_nb=%u\n",c.x,c.y,r,factor,num);

    /* Store obstacle */
    //assert(obstacles_nb < PATH_OBSTACLES_NB);
    obstacles[obstacles_nb].c = c;
    obstacles[obstacles_nb].r = r;
    obstacles[obstacles_nb].factor = factor;
    obstacles_nb++;

    /* Complex number of modulus 1 and rotation angle */
    rot_a = fixed_cos_f824(PATH_OBSTACLES_POINTS_ANGLE(num));
    rot_b = fixed_sin_f824(PATH_OBSTACLES_POINTS_ANGLE(num));

    /* Additional radius factor to allow the robot to go */
    /* from one obstacle point to its neighbor point */
    /* without collision with the obstacle avoidance circle */
    /* Radius factor is sqrt( sin(angle)^2 + 1^2 ) */
    fact_r = fixed_sqrt_uf248(
                (fixed_mul_f824(rot_b,rot_b) >> 16) +
                (1L<<8) );
    fact_r += 1;  /* To counter rounding operations */
    //fact_r += 13; /* Add 10% margin */

    /* First point is at the relative position (r;0) */
    x = ((r * fact_r) >> 8);
    y = 0;

    /* Compute obstacle points positions around a circle */
    for (j = 0; j<num; j++)
    {
        /* Compute the point absolute position */
        points[points_nb].x = c.x + (vect_value_t)x;
        points[points_nb].y = c.y + (vect_value_t)y;

        /* Check it is in playground */
        if (points[points_nb].x >= border_xmin
         && points[points_nb].y >= border_ymin
         && points[points_nb].x <= border_xmax
         && points[points_nb].y <= border_ymax)
        {
            /* Accept point */
            DPRINTF("Add point %u (%u;%u)\n", points_nb, points[points_nb].x, points[points_nb].y);
            points_nb++;
        }

        /* Complex multiply with A = cos(angle) + i sin(angle) */
        nx = fixed_mul_f824(x, rot_a) - fixed_mul_f824 (y, rot_b);
        y = fixed_mul_f824 (y, rot_a) + fixed_mul_f824 (x, rot_b);
        x = nx;
    }

#ifdef HOST
    // Plot obstacle points
    robot->hardware.simu_report.pos( &points[PATH_RESERVED_POINTS_NB], points_nb-PATH_RESERVED_POINTS_NB, PATH_PLOT_ID);
#if 0
    // Draw the last obstacle
    points[points_nb] = points[points_nb - num];
    robot->hardware.simu_report.path( &points[points_nb - num], num + 1);
#endif
#endif
}

void Path::obstacle(int index, const vect_t &c, int r, int factor)
{
    add_obstacle(c, r, factor, PATH_OBSTACLES_POINTS_NB);
}

void Path::endpoints(const vect_t &src, const vect_t &dst)
{
    DPRINTF("Set path endpoints src=(%u;%u) dst=(%u;%u)\n",
            src.x, src.y, dst.x, dst.y);
    points[PATH_POINT_SRC_IDX] = src;
    points[PATH_POINT_DST_IDX] = dst;
}

void Path::compute(int factor)
{
    DPRINTF("** Path compute(start) factor=%u\n", factor);

    /* Store the escape factor */
    escape_factor = factor;

    /* Call the A* algorithm */
    path_found = (bool)astar(astar_nodes, PATH_NODES_NB, PATH_POINT_DST_IDX, PATH_POINT_SRC_IDX);
    if (path_found)
    {
        /* Store next node to go to */
        next_node = astar_nodes[0].prev;

#if PATH_DEBUG
        /* Log and display the path found */
        vect_t path[PATH_POINTS_NB];
        uint8_t node = PATH_POINT_SRC_IDX;
        uint8_t path_nb = 0;

        DPRINTF(">> Path found: ");
        while(node!=PATH_POINT_DST_IDX)
        {
            DPRINTF("%u, ", node);
            path[path_nb++] = points[node];
            node = astar_nodes[node].prev;
        }
        path[path_nb++] = points[node];
        robot->hardware.simu_report.path(path, path_nb);
        DPRINTF("%u\n", node);
    }
#endif

    DPRINTF("** Path compute(end) found=%u\n", path_found);
}

bool Path::get_next(vect_t &p)
{
    if (path_found)
    {
        p = points[next_node];
        next_node = astar_nodes[next_node].prev;
    }
    return path_found;
}

vect_t& Path::get_point_vect(const uint8_t index)
{
    //assert(index<points_nb);
    return points[index];
}

int Path::get_point_index(const vect_t& point)
{
    for(int i=0; i<points_nb; i++)
    {
        if (points[i].x==point.x && points[i].y==point.y)
            return i;
    }
    return -1;
}

uint8_t Path::find_neighbors(uint8_t cur_point, struct astar_neighbor_t *neighbors)
{
    uint8_t neighbors_nb = 0;

    /* Parse all nodes */
    for(int i=0; i<points_nb; i++)
    {
        /* Except the current one */
        if (i!=cur_point)
        {
            /* Get segment length */
            uint16_t weight = distance_point_point(&points[cur_point], &points[i]);
            DPRINTF("- Node %u (%u;%u) w=%u ", i, points[i].x, points[i].y, weight);

            /* Check every obstacle */
            for(int j=0; j<obstacles_nb; j++)
            {
                // Check for intersection with obstacle
              uint16_t d = distance_segment_point(&points[cur_point], &points[i], &obstacles[j].c);
              if (d < obstacles[j].r)
              {
                  /* Collision: apply the escape factor if node is the source point, invalidate node otherwise */
                  weight *= (i==PATH_POINT_SRC_IDX || cur_point==PATH_POINT_DST_IDX ? escape_factor : 0);
                  DPRINTF("in collision with c=(%u;%u) r=%u w=%u ",
                      obstacles[j].c.x, obstacles[j].c.y, obstacles[j].r, weight);
                  break; /* Stop checking obstacle for this node */
              }
            }

            // Add neighbor if valid
            if (weight)
            {
                DPRINTF("=> validated w=%u\n", weight);
                neighbors[neighbors_nb].node = i;
                neighbors[neighbors_nb].weight = weight;
                neighbors_nb++;
            }
            else
            {
                DPRINTF("=> skipped\n");
            }
        }
    }

#if PATH_DEBUG
    DPRINTF("\tFound %u neighbors: ", neighbors_nb);
    for(int i=0;i<neighbors_nb;i++)
        DPRINTF("%u (%u)  ", neighbors[i].node, neighbors[i].weight);
    DPRINTF("\n");
#endif

    return neighbors_nb;
}

void Path::prepare_score(const vect_t &src, int factor)
{
    DPRINTF("Path prepare score from src=(%u;%u) factor=%u\n", src.x, src.y, factor);
    escape_factor = factor;
    astar_dijkstra_prepare(astar_nodes, PATH_POINTS_NB, get_point_index(src), PATH_POINT_DST_IDX);
}

uint16_t Path::get_score(const vect_t &dst)
{
    uint16_t score = astar_dijkstra_finish(astar_nodes, PATH_POINTS_NB, get_point_index(dst));
    DPRINTF("Path get score=%u for dst=(%u;%u)\n", score, dst.x, dst.y);
    return score;
}

extern "C" uint8_t
AC_ASTAR_NEIGHBOR_CALLBACK (uint8_t node, struct astar_neighbor_t *neighbors)
{
#if PATH_DEBUG
    vect_t point_v = robot->path.get_point_vect(node);
    DPRINTF("AC_ASTAR_NEIGHBOR_CALLBACK node=%u (%u;%u)\n", node, point_v.x, point_v.y);
#endif
    return robot->path.find_neighbors(node, neighbors);
}

extern "C" uint16_t
AC_ASTAR_HEURISTIC_CALLBACK (uint8_t node)
{
    vect_t point_a = robot->path.get_point_vect(node);
    vect_t point_b = robot->path.get_point_vect(PATH_POINT_SRC_IDX);
    int16_t dist = distance_point_point(&point_a, &point_b);
    DPRINTF("Heuristic node=%u dist=%u\n", node, dist);
    return dist;
}

