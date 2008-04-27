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

#define MOVE_OBSTACLE_RADIUS 100
#define MOVE_OBSTACLE_VALIDITY 650

/** move FSM associated data. */
struct move_data_t
{
    /* Desired x position to reached. */
    uint16_t position_x;
    /* Desired y position to reached. */
    uint16_t position_y;
    /* number of obstacles seen. */
    uint8_t nb_obstacle;
    /* Path finder position x. */
    uint16_t path_pos_x;
    /* Path finder position y. */
    uint16_t path_pos_y;
    /* Position blocked. */
    asserv_position_t pos_blocked_or_failed;
};


/** move global. */
extern struct move_data_t move_data;

/** Start a move FSM. */
void
move_start (uint32_t position_x, uint32_t position_y);

#endif /* move_h */
