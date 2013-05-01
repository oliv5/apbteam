#ifndef path_2013_hh
#define path_2013_hh
// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Olivier Lanneluc
//
// APBTeam:
//      Web: http://apbteam.org/
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

#include "path.hh"
#include "playground_2013.hh"

/** 2013 path finding class */
class Path_2013 : public Path
{
  public:
    /** Constructor */
    Path_2013();
    /** Reset path computation, remove every obstacles */
    void reset(void);
    /** Set path source and destination */
    void endpoints(const vect_t &src, const vect_t &dst, const bool force_move=false);

  private:
    /** Number of additional obstacles on the 2013 playground */
    static const int PATH_2013_OBSTACLES_NB = (PATH_CC_OBSTACLES_NB + 1/*cake*/);
    /** Number of navigation points on each layer for the cake */
    static const int PATH_2013_CAKE_NAVPOINTS_NB = 14;
    /** Number of navigation points layers for the cake. */
    static const int PATH_2013_CAKE_NAVPOINTS_LAYERS = 1;
    /** Number of additional navigation points on the 2013 playground */
    static const int PATH_2013_NAVPOINTS_NB =
        PATH_CC_NAVPOINTS_NB + PATH_2013_CAKE_NAVPOINTS_LAYERS * (PATH_2013_CAKE_NAVPOINTS_NB - PATH_2013_OBSTACLES_NB);
};

#endif // path_2013_hh
