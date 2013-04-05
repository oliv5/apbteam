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
#include "debounce.hh"

Debounce::Debounce (ucoo::Io &io, int filter_depth)
    : io_ (io), filter_depth_ (filter_depth), state_ (io.get ()), filter_ (0)
{
}

void
Debounce::update ()
{
    bool now = io_.get ();
    // State unchanged, reset filter.
    if (now == state_)
        filter_ = 0;
    // State changed, should be kept stable for filter_depth_ updates until we
    // accept it.
    else if (filter_++ == filter_depth_)
    {
        state_ = now;
        filter_ = 0;
    }
}

