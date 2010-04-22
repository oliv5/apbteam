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

#include "defs.h"

/** Real radius of an obstacle. */
#define MOVE_REAL_OBSTACLE_RADIUS 150

/** Obstacle radius for the path module.
 * It corresponds to the real radius of the obstacle plus the distance you
 * want to add to avoid it. */
#define MOVE_OBSTACLE_RADIUS (MOVE_REAL_OBSTACLE_RADIUS \
			      + RADAR_CLEARANCE_MM \
			      + BOT_SIZE_SIDE)

/** Obstacle validity time (in term of number of cycles). */
#define MOVE_OBSTACLE_VALIDITY (6 * 225)

/**
 * Move FSM associated data.
 */
struct move_data_t
{
    /** Final position. */
    position_t final;
    /** Next step. */
    vect_t step;
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
move_start (position_t position, uint8_t backward);

/** To be called when obstacles positions are computed. */
void
move_obstacles_update (void);

/** Check for blocking obstacles. */
void
move_check_obstacles (void);

#endif /* move_h */
