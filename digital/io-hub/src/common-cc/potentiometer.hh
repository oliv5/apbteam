#ifndef potentiometer_hh
#define potentiometer_hh
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
#include "i2c_queue.hh"

/// I2C potentiometer, MCP4661.
class Potentiometer : public I2cQueue::Slave
{
  public:
    /// Constructor with address variable part.
    Potentiometer (I2cQueue &queue, int a2a1a0);
    /// See I2cQueue::Slave::recv_status.
    void recv_status (const uint8_t *status);
    /// Send a low level command.
    void send_command (int mem_addr, int data);
    /// Set wiper to specified level.
    void set_wiper (int index, int data, bool eeprom = false);
};

#endif // potentiometer_hh
