#ifndef robot_hh
#define robot_hh
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
#include "hardware.hh"
#include "asserv.hh"
#include "fsm_queue.hh"

#include "ucoolib/base/proto/proto.hh"

/// Main robot superclass.
class Robot : public ucoo::Proto::Handler
{
  public:
    /// Scaling factor, millimeter per step.
    static const float scale = 0.0395840674352314;
  public:
    /// Initialise robot singleton.
    Robot ();
    /// Main program loop.
    void main_loop ();
    /// Generate events for the FSM.
    bool fsm_gen_event ();
    /// Receive proto messages.
    void proto_handle (ucoo::Proto &proto, char cmd, const uint8_t *args, int size);
  public:
    /// Public access to hardware class.
    Hardware hardware;
  private:
    /// Main I2C queue.
    I2cQueue main_i2c_queue_;
  public:
    /// Public access to asserv class.
    Asserv asserv;
  private:
    /// Proto associated to each serial interface.
    ucoo::Proto dev_proto, zb_proto, usb_proto;
  public:
    /// Public access to deferred events FSM queue.
    FsmQueue fsm_queue;
};

/// Global instance pointer.
extern Robot *robot;

#endif // robot_hh
