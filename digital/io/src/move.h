#ifndef move_h
#define move_h
/* move.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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

/* if the left border is under 500 mm of the actual position do not go there. */
#define MOVE_BORDER_LEVEL 200


/** move FSM associated data. */
struct move_data_t
{
    /* Desired x position to reached. */
    uint32_t position_x;
    /* Desired y position to reached. */
    uint32_t position_y;
};


/** move global. */
extern struct move_data_t move_data;

/** Start a move FSM. */
void
move_start (uint32_t position_x, uint32_t position_y);

/** Verify if the position desired is in the table use when the robot tries to
 * reach a point and a obstacle is in front of it.
 * \param  pos  the robot's current position on the table.
 * \param  new_pos  the position computed to go to.
 * \return  true if the it can reach the position.
 */
uint8_t
move_can_go_on_left_or_right (asserv_position_t current_pos, 
			      asserv_position_t new_pos);

/** Go to the right.
  */
void
move_go_to_right (void);

/** Go to the left.
  */
void
move_go_to_left (void);

#endif /* move_h */
