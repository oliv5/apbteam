#ifndef hardware_hh
#define hardware_hh
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

#include "ucoolib/hal/uart/uart.hh"
#ifdef TARGET_stm32
# include "ucoolib/hal/usb/usb.hh"
#else
# include "ucoolib/arch/host/host_stream.hh"
#endif

#ifdef TARGET_host
# include "ucoolib/arch/host/host.hh"

/// Class to work around C++ initializer list syntax.
class HardwareHost : public ucoo::Host
{
  public:
    HardwareHost ();
};

#endif

struct Hardware
{
#ifdef TARGET_host
    HardwareHost host;
#endif
    ucoo::Uart dev_uart, zb_uart;
#ifdef TARGET_stm32
    ucoo::UsbStreamControl usb_control;
    ucoo::UsbStream usb;
#else
    ucoo::HostStream usb;
#endif
    Hardware ();
    // Wait until next cycle.
    void wait ();
};

#endif // hardware_hh
