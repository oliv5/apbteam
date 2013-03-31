#ifndef defs_hh
#define defs_hh
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
#include "ucoolib/common.hh"

/// General purpose definitions.

extern "C" {
#include "modules/math/geometry/geometry.h"
#include "modules/math/geometry/vect.h"
}

/// Position with an angle.
struct Position
{
    /// Cartesian position in millimeters, (0, 0) is at bottom left.
    vect_t v;
    /// Angle, counter-clockwise, [0, 1), f0.16.
    /// For example, 0x8000 means 0.5, which means 180 degrees.
    uint16_t a;
};

/// Convert degrees to an angle usable in Position.
#define POSITION_A_DEG(a) G_ANGLE_UF016_DEG (a)

/// Moving direction.
enum Direction
{
    /// No direction or unknown.
    DIRECTION_NONE,
    /// Forward direction, along the robot X axis.
    DIRECTION_FORWARD,
    /// Backward direction, opposite to the robot X axis.
    DIRECTION_BACKWARD,
};

/// Team color, determine the start zone side.
enum TeamColor
{
    /// Start on the right side, every coordinates are mirrored.
    TEAM_COLOR_RIGHT = 0,
    /// Start on the left side.
    TEAM_COLOR_LEFT = 1,
    /// Initial value, should never be read.
    TEAM_COLOR_UNKNOWN = 2
};

/// Current team color, sampled early during the setup stage.
extern TeamColor team_color;

#endif // defs_hh
