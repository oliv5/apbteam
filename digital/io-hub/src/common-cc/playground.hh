#ifndef playground_hh
#define playground_hh
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
#include "debug.host.hh"

/// Define playground constants and tools.

/// Playground width, mm.
static const int pg_width = 3000;

/// Playground length, mm.
static const int pg_length = 2000;

/// Distance to playground borders for general movements, mm.
static const int pg_border_distance = 250;

/// Return mirror X coordinate, using a horizontal symmetry.
#define PG_MIRROR_X(x) (pg_width - (x))

/// Return X coordinate, applying symmetry according to team color.
static inline int
pg_x (int x)
{
    host_assert (team_color != TEAM_COLOR_UNKNOWN);
    return team_color ? x : PG_MIRROR_X (x);
}

/// Same as pg_y, but for Y coordinate.  Actually nothing is done, there is no
/// symmetry.
static inline int
pg_y (int y)
{
    return y;
}

/// Return an angle, applying symmetry according to team color.  Takes degrees
/// as input.
static inline uint16_t
pg_a_deg (double a)
{
    host_assert (team_color != TEAM_COLOR_UNKNOWN);
    return team_color ? POSITION_A_DEG (a) : POSITION_A_DEG (180 - a);
}

/// Position applying symmetry according to color.  Takes degrees for angle.
static inline Position pg_position_deg (int x, int y, double a)
{
    host_assert (team_color != TEAM_COLOR_UNKNOWN);
    if (team_color)
        return (Position) { { (int16_t) x, (int16_t) y },
            POSITION_A_DEG (a) };
    else
        return (Position) { { (int16_t) PG_MIRROR_X (x), (int16_t) y },
            POSITION_A_DEG (180 - a) };
}

/// Vector applying symmetry according to color.
static inline vect_t pg_vect (int x, int y)
{
    return (vect_t) { (int16_t) pg_x (x), (int16_t) pg_y (y) };
}

#endif // playground_hh
