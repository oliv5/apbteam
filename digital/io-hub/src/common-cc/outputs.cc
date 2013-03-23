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
#include "outputs.hh"

Outputs::Outputs (ucoo::Io *outputs[], int outputs_nb)
    : outputs_ (outputs), outputs_nb_ (outputs_nb),
      def_delay_ (0)
{
}

void
Outputs::command (Command c, uint32_t mask)
{
    for (int i = 0; i < outputs_nb_; i++)
    {
        if (mask & (1 << i))
        {
            switch (c)
            {
            case RESET:
                outputs_[i]->reset ();
                break;
            case SET:
                outputs_[i]->set ();
                break;
            case TOGGLE:
                outputs_[i]->toggle ();
                break;
            }
        }
    }
}

void
Outputs::command_later (Command c, uint32_t mask, int delay)
{
    def_command_ = c;
    def_mask_ = mask;
    def_delay_ = delay;
}

void
Outputs::update ()
{
    if (def_delay_ && !--def_delay_)
        command (def_command_, def_mask_);
}

