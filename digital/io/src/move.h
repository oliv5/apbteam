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

/**
 * A position.
 */
typedef struct move_position_t
{
    /** X position. */
    uint16_t x;
    /** Y position. */
    uint16_t y;
} move_position_t;

/**
 * Move FSM associated data.
 */
struct move_data_t
{
    /** Final position. */
    move_position_t final;
    /** Intermediate position. */
    move_position_t intermediate;
    /** Obstacle position. */
    move_position_t obstacle;
    /** Backward direction allowed flag. */
    uint8_t backward_movement_allowed;
    /** Cached moving direction of the bot when blocked. */
    uint8_t cached_moving_direction;
};

/**
 * Move global data.
 */
extern struct move_data_t move_data;

/**
 * Go to a position with the start FSM.
 * @param position_x the X position.
 * @param position_y the Y position.
 * @param backward_movement_allowed do we allow backward movement?
 */
void
move_start (uint16_t position_x, uint16_t position_y, uint8_t
	    backward_movement_allowed);

#endif /* move_h */
