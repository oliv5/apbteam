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
#include "strat.hh"

Strat::Decision
Strat::decision (vect_t &pos)
{
    // TODO: this is a stub.
    pos = pg_cake_pos;
    return CANDLES;
}

void
Strat::decision_candles (CandlesDecision &decision, uint16_t robot_angle)
{
    // TODO: this is a stub.
    if (robot_angle > G_ANGLE_UF016_DEG (-90))
    {
        decision.direction = Asserv::BACKWARD;
        decision.end_angle = G_ANGLE_UF016_DEG (180 + 180. / 6);
    }
    else
    {
        decision.direction = Asserv::FORWARD;
        decision.end_angle = G_ANGLE_UF016_DEG (-180. / 6);
    }
}

void
Strat::failure ()
{
}

