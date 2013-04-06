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
#include "asserv.hh"

#include "ucoolib/utils/bytes.hh"

Asserv::Asserv (I2cQueue &queue, float scale)
    : I2cQueue::Slave (queue, 0x04, 10 + aux_nb * 2),
      status_flag_ (0),
      input_port_ (0),
      position_x_ (0), position_y_ (0), position_a_ (0),
      last_moving_direction_ (DIRECTION_NONE),
      scale_ (scale), scale_inv_ (1 / scale)
{
}

void
Asserv::recv_status (const uint8_t *status)
{
    status_flag_ = status[0];
    input_port_ = status[1];
    position_x_ = ucoo::bytes_pack (0, status[2], status[3], status[4]);
    position_y_ = ucoo::bytes_pack (0, status[5], status[6], status[7]);
    position_a_ = ucoo::bytes_pack (status[8], status[9]);
    Direction d = get_moving_direction ();
    if (d != DIRECTION_NONE)
        last_moving_direction_ = d;
}

void
Asserv::get_position (Position &position) const
{
    position.v.x = position_x_ * scale_;
    position.v.y = position_y_ * scale_;
    position.a = position_a_;
}

void
Asserv::free ()
{
    uint8_t buf[] = { 'w' };
    send (buf, sizeof (buf));
}

void
Asserv::stop ()
{
    uint8_t buf[] = { 's' };
    send (buf, sizeof (buf));
}

void
Asserv::move_distance (int distance)
{
    distance *= scale_inv_;
    uint8_t buf[] = { 'l',
        ucoo::bytes_unpack (distance, 2),
        ucoo::bytes_unpack (distance, 1),
        ucoo::bytes_unpack (distance, 0),
    };
    send (buf, sizeof (buf));
}

void
Asserv::move_angle (int16_t angle)
{
    uint8_t buf[] = { 'a',
        ucoo::bytes_unpack (angle, 1),
        ucoo::bytes_unpack (angle, 0),
    };
    send (buf, sizeof (buf));
}

void
Asserv::goto_angle (uint16_t angle)
{
    uint8_t buf[] = { 'y',
        ucoo::bytes_unpack (angle, 1),
        ucoo::bytes_unpack (angle, 0),
    };
    send (buf, sizeof (buf));
}

void
Asserv::goto_xya (const Position &pos,
                  DirectionConsign direction_consign)
{
    int x = pos.v.x * scale_inv_;
    int y = pos.v.y * scale_inv_;
    uint8_t buf[] = { 'X',
        ucoo::bytes_unpack (x, 2),
        ucoo::bytes_unpack (x, 1),
        ucoo::bytes_unpack (x, 0),
        ucoo::bytes_unpack (y, 2),
        ucoo::bytes_unpack (y, 1),
        ucoo::bytes_unpack (y, 0),
        ucoo::bytes_unpack (pos.a, 1),
        ucoo::bytes_unpack (pos.a, 0),
        direction_consign,
    };
    send (buf, sizeof (buf));
}

void
Asserv::goto_xy (const vect_t &vect,
                 DirectionConsign direction_consign)
{
    int x = vect.x * scale_inv_;
    int y = vect.y * scale_inv_;
    uint8_t buf[] = { 'x',
        ucoo::bytes_unpack (x, 2),
        ucoo::bytes_unpack (x, 1),
        ucoo::bytes_unpack (x, 0),
        ucoo::bytes_unpack (y, 2),
        ucoo::bytes_unpack (y, 1),
        ucoo::bytes_unpack (y, 0),
        direction_consign,
    };
    send (buf, sizeof (buf));
}

void
Asserv::push_wall (DirectionConsign direction_consign, int init_x, int init_y,
                   int16_t init_a)
{
    if (init_x != -1)
        init_x *= scale_inv_;
    if (init_y != -1)
        init_y *= scale_inv_;
    uint8_t buf[] = { 'G',
        direction_consign,
        ucoo::bytes_unpack (init_x, 2),
        ucoo::bytes_unpack (init_x, 1),
        ucoo::bytes_unpack (init_x, 0),
        ucoo::bytes_unpack (init_y, 2),
        ucoo::bytes_unpack (init_y, 1),
        ucoo::bytes_unpack (init_y, 0),
        ucoo::bytes_unpack (init_a, 1),
        ucoo::bytes_unpack (init_a, 0),
    };
    send (buf, sizeof (buf));
}

void
Asserv::follow (DirectionConsign direction_consign)
{
    uint8_t buf[] = { 'o',
        direction_consign,
    };
    send (buf, sizeof (buf));
}

void
Asserv::follow_update (int16_t consign)
{
    uint8_t buf[] = {
        ucoo::bytes_unpack (consign, 1),
        ucoo::bytes_unpack (consign, 0),
    };
    send (buf, sizeof (buf), I2cQueue::TRANSIENT);
}

void
Asserv::set_speed (uint16_t linear_hi, uint16_t angular_hi, uint16_t
                   linear_lo, uint16_t angular_lo)
{
    uint8_t buf[] = { 'p', 's',
        ucoo::bytes_unpack (linear_hi, 1),
        ucoo::bytes_unpack (linear_hi, 0),
        ucoo::bytes_unpack (angular_hi, 1),
        ucoo::bytes_unpack (angular_hi, 0),
        ucoo::bytes_unpack (linear_lo, 1),
        ucoo::bytes_unpack (linear_lo, 0),
        ucoo::bytes_unpack (angular_lo, 1),
        ucoo::bytes_unpack (angular_lo, 0),
    };
    send (buf, sizeof (buf));
}

void
Asserv::set_acceleration (uint16_t linear, uint16_t angular)
{
    uint8_t buf[] = { 'p', 'a',
        ucoo::bytes_unpack (linear, 1),
        ucoo::bytes_unpack (linear, 0),
        ucoo::bytes_unpack (angular, 1),
        ucoo::bytes_unpack (angular, 0),
    };
    send (buf, sizeof (buf));
}

