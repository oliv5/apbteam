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
#include "simu_report.host.hh"

SimuReport::SimuReport (ucoo::Host &host)
    : node_ (host.get_node ())
{
    std::string instance (host.get_instance ());
    pos_mtype_ = node_.reserve (instance + ":pos-report");
    path_mtype_ = node_.reserve (instance + ":path");
}

void
SimuReport::pos (vect_t *pos, int pos_nb, uint8_t id)
{
    ucoo::mex::Msg msg (pos_mtype_);
    msg.push ("B") << id;
    for (; pos_nb; pos++, pos_nb--)
        msg.push ("hh") << pos->x << pos->y;
    node_.send (msg);
}

void
SimuReport::path (const vect_t *pos, int pos_nb)
{
    ucoo::mex::Msg msg (path_mtype_);
    for (; pos_nb; pos++, pos_nb--)
        msg.push ("hh") << pos->x << pos->y;
    node_.send (msg);
}

