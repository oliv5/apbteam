#ifndef beacon_hh
#define beacon_hh
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
#include "defs.hh"

/// Interface to beacon board.
class Beacon : public I2cQueue::Slave
{
  public:
    /// Number of reported positions.
    static const int pos_nb = 2;
  public:
    /// Constructor.
    Beacon (I2cQueue &queue);
    /// See I2cQueue::Slave::recv_status.
    void recv_status (const uint8_t *status);
    /// Turn on (true) or off (false).
    void on (bool state = true) { on_ = state; }
    /// Give number of opponents robots.
    void set_robots_nb (int robots_nb) { robots_nb_ = robots_nb; }
    /// Send my position along with other informations.
    void send_position (const vect_t &pos);
    /// Get a detected position, return trust (0 for invalid, 100 for full
    /// trust).
    int get_position (int index, vect_t &pos)
    {
        pos = pos_[index];
        return trust_[index];
    }
  private:
    /// To send: beacon system powered.
    bool on_;
    /// To send: number of opponents.
    int robots_nb_;
    /// Received obstacles position.
    vect_t pos_[pos_nb];
    /// Received trust.
    int trust_[pos_nb];
};

#endif // beacon_hh
