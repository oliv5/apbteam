#ifndef playground_2012_h
#define playground_2012_h
/* playground_2012.h */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */

#define EUROBOT 2012

#include "playground.h"

/** X offset from the table center of a totem center. */
#define PG_TOTEM_X_OFFSET_MM 400

/** Totem width. */
#define PG_TOTEM_WIDTH_MM 250

/** Totem diagonal length. */
#define PG_TOTEM_DIAG_MM 353

/** Totems position. */
#define PG_TOTEM_LEFT_X (PG_WIDTH / 2 - PG_TOTEM_X_OFFSET_MM)
#define PG_TOTEM_RIGHT_X (PG_WIDTH / 2 + PG_TOTEM_X_OFFSET_MM)
#define PG_TOTEM_Y (PG_LENGTH / 2)

/** Peanut island. */
#define PG_PEANUT_RADIUS_MM 300

/** Ships holds. */
#define PG_HOLD_SOUTH_X 325
#define PG_HOLD_SOUTH_Y 0
#define PG_HOLD_NORTH_X 362
#define PG_HOLD_NORTH_Y 740

/** Captain rooms. */
#define PG_CAPTAIN_ROOM_WIDTH_MM 400
#define PG_CAPTAIN_ROOM_LENGTH_MM 500

/** Bottles, align on path grid. */
#define PG_BOTTLE0_X (PG_TOTEM_LEFT_X - PATH_TOTEM_CLEAR_MM)
#define PG_BOTTLE1_X PG_TOTEM_LEFT_X
#define PG_BOTTLE2_X PG_TOTEM_RIGHT_X
#define PG_BOTTLE3_X (PG_TOTEM_RIGHT_X + PATH_TOTEM_CLEAR_MM)

#endif /* playground_2012_h */
