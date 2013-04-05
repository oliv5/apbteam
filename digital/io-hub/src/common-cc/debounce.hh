#ifndef debounce_hh
#define debounce_hh
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
#include "ucoolib/intf/io.hh"

/// Debounce a contact.
class Debounce
{
  public:
    /// Constructor.
    Debounce (ucoo::Io &io, int filter_depth);
    /// To be called at each cycle.
    void update ();
    /// Get filtered state.
    bool get () const { return state_; }
  private:
    /// Attached input.
    ucoo::Io &io_;
    /// Filter depth, number of updates for which input should be stable.
    int filter_depth_;
    /// Current output.
    bool state_;
    /// Current filter counter.
    int filter_;
};

#endif // debounce_hh
