#ifndef outputs_hh
#define outputs_hh
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
#include "ucoolib/common.hh"
#include "ucoolib/intf/io.hh"

/// Handle a set of outputs.
class Outputs
{
  public:
    enum Command {
        RESET,
        SET,
        TOGGLE,
    };
  public:
    /// Constructor.
    Outputs (ucoo::Io *outputs[], int outputs_nb);
    /// Apply command on a set of output now.
    void command (Command c, uint32_t mask);
    /// Apply deferred command, can only be one active.
    void command_later (Command c, uint32_t mask, int delay);
    /// To be called at each cycle to handle deferred commands.
    void update ();
  private:
    /// Output table.
    ucoo::Io **outputs_;
    /// Output table size.
    int outputs_nb_;
    /// Deferred command.
    Command def_command_;
    /// Deferred mask.
    uint32_t def_mask_;
    /// Remaining delay until deferred command, or 0 if not active.
    int def_delay_;
};

#endif // outputs_hh
