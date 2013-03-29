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
#include "pressure.hh"

#include <algorithm>

Pressure::Pressure (ucoo::Adc &sensor, ucoo::Io &pneum_open,
                    Mimot::Motor &pump)
    : sensor_ (sensor), pneum_open_ (pneum_open), pump_ (pump),
      state_ (STATE_INITIAL), wait_ (0), low_ (0)
{
}

void
Pressure::set (int low)
{
    low_ = std::min (low, max_ - margin_);
}

int
Pressure::get () const
{
    return sensor_.read ();
}

void
Pressure::update ()
{
    // Interval at which the sensor is read.
    static const int interval = 25;
    // Rest duration after a pumping session.
    static const int rest = 250;
    // Wait duration before the pump is started.
    static const int open_wait = 125;
    // Wait duration after the pump has been started before circuit is closed.
    static const int start_wait = 50;
    if (wait_)
    {
        wait_--;
    }
    else
    {
        switch (state_)
        {
        case STATE_INITIAL:
            if (low_)
            {
                // Open circuit.
                pneum_open_.set ();
                state_ = STATE_IDLE;
                wait_ = open_wait;
            }
            break;
        case STATE_IDLE:
            if (get () < low_)
            {
                // Start pump.
                pump_.output_set (Mimot::Motor::pwm_max);
                state_ = STATE_STARTING;
                wait_ = start_wait;
            }
            else
                wait_ = interval;
            break;
        case STATE_STARTING:
            // Started, close circuit.
            pneum_open_.reset ();
            state_ = STATE_PUMPING;
            break;
        case STATE_PUMPING:
            // Pump until high.
            if (get () > low_ + margin_)
            {
                // Done. Stop. Open circuit.
                pump_.free ();
                pneum_open_.set ();
                state_ = STATE_IDLE;
                wait_ = rest;
            }
            else
                wait_ = interval;
            break;
        }
    }
}

