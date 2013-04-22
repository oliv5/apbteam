#ifndef move_hh
#define move_hh
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
#include "asserv.hh"

/// Move FSM control.
class Move
{
  public:
    enum NextMove
    {
        NONE,
        LINEAR,
        ANGULAR,
    };
    /// Distance considered too small to move.
    static const int eps = 15;
  public:
    /// Go to a position.
    void start (const Position &position,
                Asserv::DirectionConsign direction_consign = Asserv::FORWARD,
                int shorten = 0);
    /// Go to a position with no angle consign.
    void start (const vect_t &position,
                Asserv::DirectionConsign direction_consign = Asserv::FORWARD,
                int shorten = 0);
    /// Stop movement.
    void stop ();
    /// Check for blocking obstacles, return true if an event was handled.
    bool check_obstacles ();
    /// Go to current step, low level function.
    void go ();
    /// Go or rotate toward position, return LINEAR or ANGULAR.
    NextMove go_or_rotate (const vect_t &dst, uint16_t angle, bool with_angle,
                           Asserv::DirectionConsign direction_consign);
    /// Go to next position computed by path module, to be called by
    /// path_init and path_next, return LINEAR or ANGULAR.
    NextMove go_to_next (const vect_t &dst);
    /// Compute path and go to first position, return NONE, LINEAR or ANGULAR.
    NextMove path_init ();
    /// Go to next position in path, return LINEAR or ANGULAR.
    NextMove path_next ();
  public:
    /// Final position.
    Position final_;
    /// Use angle consign for final point.
    bool with_angle_;
    /// Direction consign of the whole movement.
    Asserv::DirectionConsign direction_consign_;
    /// Distance to remove from path.
    int shorten_;
    /// Next step.
    vect_t step_;
    /// Next step angle.
    uint16_t step_angle_;
    /// Next step with_angle.
    bool step_with_angle_;
    /// Next step direction consign.
    Asserv::DirectionConsign step_direction_consign_;
    /// Next step is the final move.
    bool step_final_move_;
    /// If true, this means this is a tricky move, slow down, and minimize
    /// turns.
    bool slow_;
};

#endif // move_hh
