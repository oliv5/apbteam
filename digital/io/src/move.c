/* move.c */
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
#include "common.h"
#include "move.h"
#include "fsm.h"
#include "bot.h"
#include "radar.h"
#include "asserv.h"
#include "main.h"

#include "modules/path/path.h"
#include "modules/utils/utils.h"

/**
 * Internal data used by the move FSM.
 */
struct move_data_t move_data;

void
move_start (position_t position, uint8_t backward)
{
    /* Set parameters. */
    move_data.final= position;
    move_data.backward_movement_allowed = backward;
    move_data.final_move = 0;
    /* Start the FSM. */
    fsm_handle_event (&ai_fsm, AI_EVENT_move_start);
}

void
move_obstacles_update (void)
{
    uint8_t i;
    for (i = 0; i < main_obstacles_nb; i++)
	path_obstacle (i, main_obstacles_pos[i], MOVE_OBSTACLE_RADIUS, 0,
		       MOVE_OBSTACLE_VALIDITY);
}

uint8_t
move_check_obstacles (void)
{
    if (fsm_can_handle_event (&ai_fsm, AI_EVENT_obstacle_in_front))
      {
	position_t robot_pos;
	asserv_get_position (&robot_pos);
	if (radar_blocking (&robot_pos.v, &move_data.step, main_obstacles_pos,
			    main_obstacles_nb))
	    if (fsm_handle_event (&ai_fsm, AI_EVENT_obstacle_in_front))
		return 1;
      }
    return 0;
}

