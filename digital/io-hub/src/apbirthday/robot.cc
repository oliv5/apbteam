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
#include "robot.hh"

#include "bot.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/utils/bytes.hh"

Robot *robot;

Robot::Robot ()
    : main_i2c_queue_ (hardware.main_i2c),
      asserv (main_i2c_queue_, BOT_SCALE),
      dev_proto (*this, hardware.dev_uart),
      zb_proto (*this, hardware.zb_uart),
      usb_proto (*this, hardware.usb)
{
    robot = this;
}

void
Robot::main_loop ()
{
    while (1)
    {
        // Wait until next cycle.
        hardware.wait ();
        // Handle communications.
        bool sync = main_i2c_queue_.sync ();
        // Handle events if synchronised.
        if (sync)
            fsm_gen_event ();
        // Handle commands.
        dev_proto.accept ();
        zb_proto.accept ();
        usb_proto.accept ();
    }
}

bool
Robot::fsm_gen_event ()
{
    // If an event is handled, stop generating any other event, because a
    // transition may have invalidated the current robot state.
#define fsm_handle_and_return(event) \
    do { if (ANGFSM_HANDLE (AI, event)) return true; } while (0)
    // FSM timeouts.
    if (FSM_HANDLE_TIMEOUT (AI))
        return true;
    // Motor status.
    Motor::Status robot_move_status;
    robot_move_status = asserv.get_status ();
    if (robot_move_status == Motor::SUCCESS)
        fsm_handle_and_return (robot_move_success);
    else if (robot_move_status == Motor::FAILURE)
        fsm_handle_and_return (robot_move_failure);
    // Jack. TODO: bounce filter.
    if (!hardware.raw_jack.get ())
        fsm_handle_and_return (jack_inserted);
    else
        fsm_handle_and_return (jack_removed);
    // FSM queue.
    if (fsm_queue.poll ())
    {
        FsmQueue::Event event = fsm_queue.pop ();
        if (ANGFSM_HANDLE_VAR (AI, event))
            return true;
    }
    return false;
}

void
Robot::proto_handle (ucoo::Proto &proto, char cmd, const uint8_t *args, int size)
{
#define c(cmd, size) ((cmd) << 8 | (size))
    switch (c (cmd, size))
    {
    case c ('z', 0):
        // Reset.
        ucoo::arch_reset ();
        break;
    case c ('m', 5):
        // Go to position.
        // 2H: x, y.
        // 1B: direction_consign.
        {
            vect_t pos = {
                (int16_t) ucoo::bytes_pack (args[0], args[1]),
                (int16_t) ucoo::bytes_pack (args[2], args[3]),
            };
            asserv.stop ();
            // TODO: use move FSM.
            asserv.goto_xy (pos, Asserv::DirectionConsign (args[4]));
        }
        break;
    default:
        proto.send ('?');
        return;
    }
    // Acknowledge.
    proto.send_buf (cmd, args, size);
}

