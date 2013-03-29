#ifndef pressure_hh
#define pressure_hh
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
#include "ucoolib/intf/adc.hh"
#include "ucoolib/intf/io.hh"
#include "mimot.hh"

/// Handle pressure sensor and pump.
class Pressure
{
  public:
    /// Constructor.
    Pressure (ucoo::Adc &sensor, ucoo::Io &pneum_open, Mimot::Motor &pump);
    /// Set desired pressure (set low threshold).
    void set (int low);
    /// Get current pressure.
    int get () const;
    /// To be called at each cycle.
    void update ();
  private:
    ucoo::Adc &sensor_;
    ucoo::Io &pneum_open_;
    Mimot::Motor &pump_;
    /// Maximum pressure.
    static const int max_ = 4000;
    /// Hysteresis margin.
    static const int margin_ = max_ / 16;
    /// Pressure FSM state.
    enum State
    {
        /// Start state, circuit is closed, no pressure.
        STATE_INITIAL,
        /// Idle, pressure is OK, circuit is open.
        STATE_IDLE,
        /// Starting the pump, with open circuit.
        STATE_STARTING,
        /// Pumping until the pressure is OK, circuit is closed.
        STATE_PUMPING,
    };
    State state_;
    /// Wait counter before next action.
    int wait_;
    /// Pressure sensor low threshold.
    int low_;
};

#endif // pressure_hh
