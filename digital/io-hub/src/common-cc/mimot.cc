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
#include "mimot.hh"

#include "ucoolib/utils/bytes.hh"

Mimot::Mimot (I2cQueue &queue)
    : I2cQueue::Slave (queue, 0x06, 6),
      motor0 (*this, 0), motor1 (*this, 1),
      status_flag_ (0), input_port_ (0)
{
}

void
Mimot::recv_status (const uint8_t *status)
{
    status_flag_ = status[0];
    input_port_ = status[1];
    motor0.position_ = ucoo::bytes_pack (status[2], status[3]);
    motor1.position_ = ucoo::bytes_pack (status[4], status[5]);
}

void
Mimot::Motor::move (uint16_t position, uint16_t speed)
{
    uint8_t buf[] = { (char) ('b' + index_),
        ucoo::bytes_unpack (position, 1),
        ucoo::bytes_unpack (position, 0),
        ucoo::bytes_unpack (speed, 1),
        ucoo::bytes_unpack (speed, 0),
    };
    mimot_.send (buf, sizeof (buf));
}

void
Mimot::Motor::free ()
{
    uint8_t buf[] = { 'w',
        (uint8_t) index_,
        0, // brake
    };
    mimot_.send (buf, sizeof (buf));
}

void
Mimot::Motor::output_set (int16_t pwm)
{
    uint8_t buf[] = { 'W',
        (uint8_t) index_,
        ucoo::bytes_unpack (pwm, 1),
        ucoo::bytes_unpack (pwm, 0),
    };
    mimot_.send (buf, sizeof (buf));
}

