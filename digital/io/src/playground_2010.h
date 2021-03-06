#ifndef playground_2010_h
#define playground_2010_h
/* playground_2010.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Dufour Jérémy
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

#define EUROBOT 2010

#include "playground.h"

/**
 * Eurobot 2010 specific defines.
 */

/**
 * Start zone.
 */
#define PG_START_ZONE_LENGTH 500
#define PG_START_ZONE_WIDTH 500

/** Size of the unclimbable slope zone. */
#define PG_SLOPE_WIDTH (500 + 519 + 500)
#define PG_SLOPE_LENGTH (500 + 22)

/** Start of field zone. */
#define PG_FIELD_Y_MAX 1128

#endif /* playground_2010_h */
