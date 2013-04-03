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
#include "beacon.hh"

#include "ucoolib/utils/bytes.hh"

Beacon::Beacon (I2cQueue &queue)
    : I2cQueue::Slave (queue, 0x0a, 5 * pos_nb), on_ (false), robots_nb_ (0)
{
    for (int i = 0; i < pos_nb; i++)
        trust_[i] = 0;
}

void
Beacon::recv_status (const uint8_t *status)
{
    int index = 0;
    for (int i = 0; i < pos_nb; i++)
    {
        pos_[i].x = ucoo::bytes_pack (status[index], status[index + 1]);
        index += 2;
        pos_[i].y = ucoo::bytes_pack (status[index], status[index + 1]);
        index += 2;
        trust_[i] = status[index];
        index++;
    }
}

void
Beacon::send_position (const vect_t &pos)
{
    uint8_t buf[] = {
        on_,
        team_color,
        (uint8_t) robots_nb_,
        ucoo::bytes_unpack (pos.x, 1),
        ucoo::bytes_unpack (pos.x, 0),
        ucoo::bytes_unpack (pos.y, 1),
        ucoo::bytes_unpack (pos.y, 0),
    };
    send (buf, sizeof (buf));
}

