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
    };
    /// Information on a candle decision.
    struct CandlesDecision
    {
        /// Movement direction, 1 (trigo) or -1 (antitrigo).
        int dir_sign;
        /// Angle relative to cake to end the movement.
        uint16_t end_angle;
    };
  public:
    /// Return new decision and associated position.
    Decision decision (vect_t &pos);
    /// Take a decision related to candles, return false to give up candles.
    bool decision_candles (CandlesDecision &decision,
                           uint16_t robot_angle);
    /// Report a failure to apply the previous decision.
    void failure ();
};

#endif // strat_hh
