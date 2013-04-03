#ifndef path_hh
#define path_hh
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
#include "defs.hh"

/// Path finding class.
/// TODO: dummy implementation.
class Path
{
  public:
    void reset () { }
    void obstacle (int index, vect_t c, int r, int factor = 0) { }
    void endpoints (const vect_t &src, const vect_t &dst) { dst_ = dst; }
    void compute (int factor = 0) { }
    bool get_next (vect_t &p) { p = dst_; return true; }
  private:
    vect_t dst_;
};

#endif // path_hh
