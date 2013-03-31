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
#include "obstacles.hh"

extern "C" {
#include "modules/math/geometry/distance.h"
}

#ifdef TARGET_host
# include "robot.hh"
#endif

Obstacles::Obstacles ()
    : changed_ (false)
{
    for (int i = 0; i < obstacles_nb_; i++)
        obstacles_[i].valid = 0;
}

void
Obstacles::update ()
{
    for (int i = 0; i < obstacles_nb_; i++)
    {
        if (obstacles_[i].valid)
            obstacles_[i].valid--;
    }
#ifdef TARGET_host
    if (changed_)
    {
        SimuReport &r = robot->hardware.simu_report;
        vect_t o[obstacles_nb_];
        int o_nb = 0;
        for (int i = 0; i < obstacles_nb_; i++)
        {
            if (obstacles_[i].valid)
                o[o_nb++] = obstacles_[i].pos;
        }
        r.pos (o, o_nb, 0);
    }
#endif
    changed_ = false;
}

void
Obstacles::add (const vect_t &pos)
{
    // Try to merge, and find the oldest on the way.
    int oldest_i = 0, oldest_i_valid = valid_new_;
    for (int i = 0; i < obstacles_nb_; i++)
    {
        if (obstacles_[i].valid)
        {
            if (distance_point_point (&pos, &obstacles_[i].pos) < same_mm_)
            {
                // Replace if older.
                if (obstacles_[i].valid < valid_new_)
                {
                    obstacles_[i].pos = pos;
                    obstacles_[i].valid = valid_new_;
                    changed_ = true;
                }
                return;
            }
        }
        if (obstacles_[i].valid < oldest_i_valid)
        {
            oldest_i = i;
            oldest_i_valid = obstacles_[i].valid;
        }
    }
    // If not found, replace the oldest.
    obstacles_[oldest_i].pos = pos;
    obstacles_[oldest_i].valid = valid_new_;
    changed_ = true;
}

bool
Obstacles::blocking (const vect_t &robot, const vect_t &dest) const
{
    // TODO
    return false;
}

