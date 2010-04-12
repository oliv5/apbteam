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

#include "modules/math/geometry/vect.h"
#include "asserv.h"

/**
 * A position.
 */
typedef struct move_position_t
{
    /** X position. */
    int16_t x;
    /** Y position. */
    int16_t y;
    /** A angle. */
    uint16_t a;
} move_position_t;

/**
 * Move FSM associated data.
 */
struct move_data_t
{
    /** Final position. */
    move_position_t final;
    /** Backward direction allowed flag. */
    uint8_t backward_movement_allowed;
    /** Try again counter. */
    uint8_t try_again_counter;
    /** Dirty fix to know this is the final move. */
    uint8_t final_move;
};

/**
 * Move global data.
 */
extern struct move_data_t move_data;

/**
 * Go to a position with the start FSM.
 * @param position the destination position.
 * @param backward_movement_allowed 0, no backward, 1 backward is allowed, 2
 * backward is compulsary.
 */
void
move_start (asserv_position_t position, uint8_t backward);

#endif /* move_h */
