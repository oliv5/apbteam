#ifndef init_defs_h
#define init_defs_h
/* init_defs.h */
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

#include "playground_2012.h"
#include "bot.h"

/** Parameters to push the first wall. */
#define INIT_FIRST_WALL_PUSH \
    ASSERV_BACKWARD, PG_X (BOT_BACK_CONTACT_DIST_MM), 250, \
    PG_A_DEG (0 + BOT_BACK_CONTACT_ANGLE_ERROR_DEG)
/** Parameters to go away from the first wall. */
#define INIT_FIRST_WALL_AWAY (250 - BOT_BACK_CONTACT_DIST_MM)
/** Parameter to face the second wall. */
#define INIT_SECOND_WALL_ANGLE PG_A_DEG (-90)
/** Parameters to push the second wall. */
#define INIT_SECOND_WALL_PUSH \
    ASSERV_BACKWARD, -1, PG_Y (PG_LENGTH - BOT_BACK_CONTACT_DIST_MM), -1
/** Parameters to go away from the second wall. */
#define INIT_SECOND_WALL_AWAY (250 - BOT_BACK_CONTACT_DIST_MM)
/** Parameter to face the start position. */
#undef INIT_START_POSITION_ANGLE
/** Start position. */
#define INIT_START_POSITION \
    PG_X (250), PG_Y (PG_LENGTH - 250), PG_A_DEG (0), ASSERV_REVERT_OK

#endif /* init_defs_h */
