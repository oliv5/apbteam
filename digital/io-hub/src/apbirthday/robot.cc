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
    : candles (1),
      main_i2c_queue_ (hardware.main_i2c),
      asserv (main_i2c_queue_, BOT_SCALE),
      dev_proto (*this, hardware.dev_uart),
      zb_proto (*this, hardware.zb_uart),
      usb_proto (*this, hardware.usb),
      chrono (90000 - 1000),
      fsm_debug_state_ (FSM_DEBUG_RUN),
      outputs_set_ (outputs_, lengthof (outputs_)),
      stats_proto_ (0),
      stats_chrono_ (false), stats_chrono_last_s_ (-1),
      stats_inputs_ (0), stats_usdist_ (0)
{
    robot = this;
    unsigned i = 0;
    inputs_[i++] = &hardware.raw_jack;
    inputs_[i++] = &hardware.ihm_color;
    inputs_[i++] = &hardware.ihm_strat;
    inputs_[i++] = &hardware.ihm_robot_nb;
    inputs_[i++] = &hardware.ihm_lol;
    inputs_[i++] = &hardware.ihm_emerg_stop;
    inputs_[i++] = &hardware.glass_contact;
    inputs_[i++] = &hardware.cherry_plate_left_contact;
    inputs_[i++] = &hardware.cherry_plate_right_contact;
    ucoo::assert (i == lengthof (inputs_));
    i = 0;
    outputs_[i++] = &hardware.cherry_bad_out;
    outputs_[i++] = &hardware.cherry_bad_in;
    outputs_[i++] = &hardware.cherry_plate_up;
    outputs_[i++] = &hardware.cherry_plate_down;
    outputs_[i++] = &hardware.cherry_plate_clamp;
    outputs_[i++] = &hardware.cake_arm_out;
    outputs_[i++] = &hardware.cake_arm_in;
    outputs_[i++] = &hardware.cake_push_far_out;
    outputs_[i++] = &hardware.cake_push_far_in;
    outputs_[i++] = &hardware.cake_push_near_out;
    outputs_[i++] = &hardware.cake_push_near_in;
    outputs_[i++] = &hardware.glass_lower_clamp_close;
    outputs_[i++] = &hardware.glass_lower_clamp_open;
    outputs_[i++] = &hardware.glass_upper_clamp_close;
    outputs_[i++] = &hardware.glass_upper_clamp_open;
    outputs_[i++] = &hardware.glass_upper_clamp_up;
    outputs_[i++] = &hardware.glass_upper_clamp_down;
    outputs_[i++] = &hardware.gift_out;
    outputs_[i++] = &hardware.gift_in;
    outputs_[i++] = &hardware.ballon_funny_action;
    outputs_[i++] = &hardware.pneum_open;
    ucoo::assert (i == lengthof (outputs_));
    for (i = 0; i < lengthof (outputs_); i++)
        outputs_[i]->output ();
}

void
Robot::main_loop ()
{
    while (1)
    {
        // Wait until next cycle.
        hardware.wait ();
        // Update IO modules.
        outputs_set_.update ();
        // Handle communications.
        bool sync = main_i2c_queue_.sync ();
        // Handle events if synchronised.
        if (sync && fsm_debug_state_ != FSM_DEBUG_STOP)
        {
            if (fsm_gen_event ()
                && fsm_debug_state_ == FSM_DEBUG_STEP)
                fsm_debug_state_ = FSM_DEBUG_STOP;
        }
        // Handle commands.
        dev_proto.accept ();
        zb_proto.accept ();
        usb_proto.accept ();
        // Handle zb programmation.
        hardware.zb_handle ();
        // Send stats.
        proto_stats ();
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
    case c ('f', 0):
        // Enter FSM debug mode, then step once.
        fsm_debug_state_ = FSM_DEBUG_STEP;
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
      case c ('o', 5):
	// Set/clear/toggle outputs.
	// - 1d: mask.
	// - 1b: 00 to clear, 01 to set, 02 to toggle.
        {
            uint32_t mask =
                ucoo::bytes_pack (args[0], args[1], args[2], args[3]);
            if (args[4] < 3)
                outputs_set_.command (Outputs::Command (args[4]), mask);
            else
            {
                proto.send ('?');
                return;
            }
        }
	break;
      case c ('o', 6):
	// Toggle outputs for a short time.
	// - 1d: mask.
	// - 1w: duration.
        {
            uint32_t mask =
                ucoo::bytes_pack (args[0], args[1], args[2], args[3]);
            outputs_set_.command (Outputs::TOGGLE, mask);
            outputs_set_.command_later (Outputs::TOGGLE, mask,
                                        ucoo::bytes_pack (args[4], args[5]));
        }
	break;
    case c ('C', 1):
        // Chrono stats.
        // 1B: start chrono if non-zero.
        stats_chrono_ = true;
        if (args[0])
            chrono.start ();
        stats_proto_ = &proto;
        break;
    case c ('I', 1):
        // Input stats.
        // 1B: stat interval.
        stats_inputs_cpt_ = stats_inputs_ = args[0];
        stats_proto_ = &proto;
        break;
    case c ('U', 1):
        // US distance sensors stats.
        // 1B: stat interval.
        stats_usdist_cpt_ = stats_usdist_ = args[0];
        stats_proto_ = &proto;
        break;
    default:
        proto.send ('?');
        return;
    }
    // Acknowledge.
    proto.send_buf (cmd, args, size);
}

void
Robot::proto_stats ()
{
    if (!stats_proto_)
        return;
    if (stats_chrono_)
    {
        int s = chrono.remaining_time_ms () / 1000;
        if (s != stats_chrono_last_s_)
        {
            stats_proto_->send ('C', "b", s);
            stats_chrono_last_s_ = s;
        }
    }
    if (stats_inputs_ && !--stats_inputs_cpt_)
    {
        uint32_t inputs = 0;
        for (unsigned int i = 0; i < lengthof (inputs_); i++)
        {
            if (inputs_[i]->get ())
                inputs |= 1 << i;
        }
        stats_proto_->send ('I', "L", inputs);
        stats_inputs_cpt_ = stats_inputs_;
    }
    if (stats_usdist_ && !--stats_usdist_cpt_)
    {
        // TODO: this is a hack, only work in simulation.
        stats_proto_->send ('U', "HHHH",
                            hardware.adc_dist0.read (),
                            hardware.adc_dist1.read (),
                            hardware.adc_dist2.read (),
                            hardware.adc_dist3.read ());
        stats_usdist_cpt_ = stats_usdist_;
    }
}

