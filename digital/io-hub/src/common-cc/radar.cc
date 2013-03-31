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
#include "radar.hh"

/// Maximum distance for a sensor reading to be ignored if another sensor is
// nearer.
static const int far_mm = 250;

void
Radar::update (const Position &robot_pos, Obstacles &obstacles)
{
    int i, j;
    vect_t ray;
    // Compute hit points for each sensor and eliminate invalid ones.
    vect_t hit[sensors_nb_];
    int dist_mm[sensors_nb_];
    for (i = 0; i < sensors_nb_; i++)
    {
        dist_mm[i] = sensors_[i].sensor->get ();
        if (dist_mm[i] != -1)
        {
            hit[i] = sensors_[i].pos;
            vect_rotate_uf016 (&hit[i], robot_pos.a);
            vect_translate (&hit[i], &robot_pos.v);
            vect_from_polar_uf016 (&ray, dist_mm[i],
                                   robot_pos.a + sensors_[i].a);
            vect_translate (&hit[i], &ray);
            if (valid (i, hit[i]))
            {
                vect_from_polar_uf016 (&ray, obstacle_edge_radius_mm_,
                                       robot_pos.a + sensors_[i].a);
                vect_translate (&hit[i], &ray);
            }
            else
                dist_mm[i] = -1;
        }
    }
    // Ignore sensor results too far from other sensors of the same group.
    for (i = 0; i < sensors_nb_ - 1; i++)
    {
        if (dist_mm[i] != -1)
        {
            for (j = i + 1; j < sensors_nb_ && !sensors_[j].new_group; j++)
            {
                if (dist_mm[j] != -1)
                {
                    if (dist_mm[i] + far_mm < dist_mm[j])
                        dist_mm[j] = -1;
                    else if (dist_mm[j] + far_mm < dist_mm[i])
                        dist_mm[i] = -1;
                }
            }
        }
    }
    // Compute hit point from all sensors in the same group.
    vect_t hit_center = { 0, 0 };
    int hit_nb = 0;
    for (i = 0; i < sensors_nb_; i++)
    {
        if (dist_mm[i] != -1)
        {
            vect_add (&hit_center, &hit[i]);
            hit_nb++;
        }
        // If last of group or last sensor, may add obstacle.
        if ((i == sensors_nb_ - 1 || sensors_[i + 1].new_group) && hit_nb)
        {
            if (hit_nb > 1)
            {
                hit_center.x /= hit_nb;
                hit_center.y /= hit_nb;
            }
            obstacles.add (hit_center);
            hit_center.x = 0;
            hit_center.y = 0;
            hit_nb = 0;
        }
    }
}

