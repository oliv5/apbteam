#ifndef strat_hh
#define strat_hh
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
#include "playground_2013.hh"

/// High level strategy decision making.
class Strat
{
  public:
    enum Decision
    {
        CANDLES,
        PLATE,
    };
    /// Information on a candle decision.
    struct CandlesDecision
    {
        /// Movement direction, 1 (trigo) or -1 (antitrigo).
        int dir_sign;
        /// Angle relative to cake to end the movement.
        uint16_t end_angle;
    };
    /// Information on a plate decision.
    struct PlateDecision
    {
        /// Should drop plate before.
        bool drop;
        /// Approach position, where the robot should be before starting
        /// approaching.
        Position approaching_pos;
        /// Loading position, point where to go backward to load the plate. If
        /// the point is reached, there is no plate.
        vect_t loading_pos;
    };
  public:
    /// Return new decision and associated position.
    Decision decision (Position &pos);
    /// Take a decision related to candles, return false to give up candles.
    bool decision_candles (CandlesDecision &decision,
                           uint16_t robot_angle);
    /// Take a decision related to plate.
    void decision_plate (PlateDecision &decision);
    /// Report a failure to apply the previous decision.
    void failure ();
  private:
    /// Last plate decision.
    PlateDecision plate_decision_;
};

#endif // strat_hh
