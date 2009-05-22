#ifndef top_h
#define top_h
/* top.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Haller
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

#include "asserv.h"

/**
 * Initialize top module.
 */
void
top_init (void);

/**
 * Get the next position to get pucks on the ground.
 * About the @p restart parameter, you need to set to 1 when for example, you
 * have unload some pucks and restart the procedure to get new one.
 * @param position the next position computed.
 * @param restart set it to 1 when you restart a procedure.
 * @return  0 if there is no more position.
 */
uint8_t
top_get_next_position_to_get_puck_on_the_ground (asserv_position_t *position,
						 uint8_t restart);

/**
 * Get the next position to get pucks from the distributor.
 * @param clean_position the position to clean the distributor.
 * @param front_position the position in front of the distributor to fuck it.
 */
void
top_get_next_position_to_get_distributor (asserv_position_t *clean_position,
					  asserv_position_t *front_position);

/**
 * Get the next position to get an unload position.
 * @param position the next unloading position.
 */
void
top_get_next_position_to_unload_puck (asserv_position_t *position);

/**
 * Count number of pucks we have taken during a match.
 */
extern uint8_t top_total_puck_taken;

/**
 * Number of pucks inside the bot.
 */
extern uint8_t top_puck_inside_bot;

#endif /* top_h */
