#ifndef drinks_hh
#define drinks_hh

// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Jerome Jutteau
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

class Drinks
{
    public:
        Drinks ();
        int nb ();
        static void add ();
        static void clear ();
        // GPIO manipulation.
        static void upper_open ();
        static void upper_close ();
        static void upper_up ();
        static void upper_down ();
        static void upper_unleach ();
        static void lower_open ();
        static void lower_close ();
    private:
        static const int total_drinks = 12;
        int nb_;
};

#endif // drinks_hh
