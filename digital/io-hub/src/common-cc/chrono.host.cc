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
#include "chrono.hh"

#include "robot.hh"

Chrono::Chrono (int duration_ms)
    : duration_ms_ (duration_ms), started_ (false)
{
}

void
Chrono::start ()
{
    started_ = true;
    start_date_ = robot->hardware.host.get_node ().date ();
}

int
Chrono::remaining_time_ms () const
{
    int now = 0;
    if (started_)
        now = robot->hardware.host.get_node ().date () - start_date_;
    return duration_ms_ - now;
}

