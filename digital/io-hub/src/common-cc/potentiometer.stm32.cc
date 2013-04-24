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
#include "potentiometer.stm32.hh"

Potentiometer::Potentiometer (I2cQueue &queue, int a2a1a0)
    : I2cQueue::Slave (queue, 0x50 | (a2a1a0 << 1), 0)
{
}

void
Potentiometer::recv_status (const uint8_t *status)
{
}

void
Potentiometer::send_command (int mem_addr, int data)
{
    uint8_t buf[] = {
        (uint8_t) ((mem_addr << 4) | ((data >> 8) & 1)),
        (uint8_t) (data & 0xff),
    };
    send (buf, sizeof (buf), I2cQueue::RAW);
}

void
Potentiometer::set_wiper (int index, int data, bool eeprom)
{
    send_command (index + (eeprom ? 2 : 0), data);
}

