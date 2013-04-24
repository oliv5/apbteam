#ifndef potentiometer_host_hh
#define potentiometer_host_hh
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
#include "ucoolib/arch/host/host.hh"
#include "defs.hh"

/// Potentiometer, Host stub.
class PotentiometerHost
{
  public:
    /// Constructor.
    PotentiometerHost (ucoo::Host &host);
    /// Set wiper to specified level.
    void set_wiper (int index, int data, bool eeprom = false);
  private:
    ucoo::mex::Node &node_;
    ucoo::mex::mtype_t mtype_;
};

#endif // potentiometer_host_hh
