#ifndef candles_hh
#define candles_hh

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

#define CANDLE_FAR 8
#define CANDLE_NEAR 12
#define CANDLES_NB (CANDLE_NEAR + CANDLE_FAR)
#define CANDLE_IS_FAR(c) ((c) < CANDLE_FAR)
#define CANDLE_IS_NEAR(c) ((c) >= CANDLE_FAR)

/* The cake is a lie !

-----------------------------------------------------------
-  8      0                                    7     19   -
 -                                                        -
  -   9      1                             6       18    -
    -                                                   -
      -   10      2                   5         17    -
        -                3      4                   -
          -    11                        16      -
             -       12   13   14   15      -
                    -     ______    -
 */

typedef enum
{
    CANDLE_UNPUNCHED,
    CANDLE_PUNCHED,
    CANDLE_STATE_NB
} candle_state_t;

typedef enum
{
    FLOOR_NEAR = 0,
    FLOOR_FAR = 1,
    CANDLE_FLOOR_NB
} candle_floor_t;

/* XXX use another common type for colors ? */
typedef enum
{
    CANDLE_RED = TEAM_COLOR_RIGHT,
    CANDLE_BLUE = TEAM_COLOR_LEFT,
    CANDLE_WHITE,
    CANDLE_UNKNOWN,
} candle_color_t;

class Candles
{
    public:
        candle_state_t state[CANDLES_NB];
        candle_color_t color[CANDLES_NB];
        Candles (int calif_mode);
        void blow (int candle);
        void deduce ();
        int actual_pos[CANDLE_FLOOR_NB];
};

#endif // candles_hh
