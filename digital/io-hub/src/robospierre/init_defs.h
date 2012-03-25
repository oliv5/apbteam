#ifndef init_defs_h
#define init_defs_h
/* init_defs.h */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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

#include "playground_2011.h"
#include "bot.h"

/** Parameters to push the first wall. */
#define INIT_FIRST_WALL_PUSH \
    0, PG_X (BOT_FRONT_CONTACT_DIST_MM), 200, \
    PG_A_DEG (180 + BOT_FRONT_CONTACT_ANGLE_ERROR_DEG)
/** Parameters to go away from the first wall. */
#define INIT_FIRST_WALL_AWAY -500
/** Parameter to face the second wall. */
#define INIT_SECOND_WALL_ANGLE PG_A_DEG (90)
/** Parameters to push the second wall. */
#define INIT_SECOND_WALL_PUSH \
    0, -1, PG_Y (PG_LENGTH - BOT_FRONT_CONTACT_DIST_MM), -1
/** Parameters to go away from the second wall. */
#define INIT_SECOND_WALL_AWAY -(200 - BOT_FRONT_CONTACT_DIST_MM)
/** Parameter to face the start position. */
#define INIT_START_POSITION_ANGLE PG_A_DEG (0)
/** Start position. */
#define INIT_START_POSITION \
    PG_X (200), PG_Y (PG_LENGTH - 200), PG_A_DEG (0), ASSERV_BACKWARD

#endif /* init_defs_h */
