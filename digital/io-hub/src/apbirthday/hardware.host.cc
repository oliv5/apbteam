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

HardwareHost::HardwareHost ()
    : Host ("io-hub0")
{
    parse_options ();
}

Hardware::Hardware ()
    : dev_uart (), zb_uart ("zb_uart"), usb ("usb"),
      main_i2c (host, 0),
      raw_jack (host, "raw_jack"),
      ihm_color (host, "ihm_color"),
      ihm_strat (host, "ihm_strat"),
      ihm_robot_nb (host, "ihm_robot_nb"),
      ihm_lol (host, "ihm_lol"),
      ihm_emerg_stop (host, "ihm_emerg_stop"),
      glass_contact (host, "glass_contact"),
      cherry_plate_left_contact (host, "cherry_plate_left_contact"),
      cherry_plate_right_contact (host, "cherry_plate_right_contact"),
      cherry_bad_out (host, "cherry_bad_out"),
      cherry_bad_in (host, "cherry_bad_in"),
      cherry_plate_up (host, "cherry_plate_up"),
      cherry_plate_down (host, "cherry_plate_down"),
      cherry_plate_clamp (host, "cherry_plate_clamp"),
      cake_arm_out (host, "cake_arm_out"),
      cake_arm_in (host, "cake_arm_in"),
      cake_push_far_out (host, "cake_push_far_out"),
      cake_push_far_in (host, "cake_push_far_in"),
      cake_push_near_out (host, "cake_push_near_out"),
      cake_push_near_in (host, "cake_push_near_in"),
      glass_lower_clamp_close (host, "glass_lower_clamp_close"),
      glass_lower_clamp_open (host, "glass_lower_clamp_open"),
      glass_upper_clamp_close (host, "glass_upper_clamp_close"),
      glass_upper_clamp_open (host, "glass_upper_clamp_open"),
      glass_upper_clamp_up (host, "glass_upper_clamp_up"),
      glass_upper_clamp_down (host, "glass_upper_clamp_down"),
      gift_out (host, "gift_out"),
      gift_in (host, "gift_in"),
      ballon_funny_action (host, "ballon_funny_action"),
      pneum_open (host, "pneum_open")
{
    dev_uart.block (false);
    zb_uart.block (false);
    usb.block (false);
}

void
Hardware::wait ()
{
    ucoo::mex::Node &node = host.get_node ();
    node.wait (node.date () + 4);
}

void
Hardware::zb_handle ()
{
    // No zb on host.
}

