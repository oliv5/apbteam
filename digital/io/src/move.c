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
#include "asserv.h"
#include "move.h"
#include "fsm.h"

#define MOVE_BORDER_END 500

#define MOVE_ANGLE_0 0x0
#define MOVE_ANGLE_90 0x4000
#define MOVE_ANGLE_180 0x8000
#define MOVE_ANGLE_270 0xC000


struct move_data_t move_data;

/** Start a move FSM. */
void
move_start (uint32_t position_x, uint32_t position_y)
{
    /* Set parameters. */
    move_data.position_x = position_x;
    move_data.position_y = position_y;
    /* Start the FSM. */
    fsm_init (&move_fsm);
    fsm_handle_event (&move_fsm, MOVE_EVENT_start);
}

/** Verify if the position desired is in the table use when the robot tries to
 * reach a point and a obstacle is in front of it.
 * \param  pos  the robot's current position on the table.
 * \param  new_pos  the position desired by the user.
 * \return  true if the it can reach the position.
 */
uint8_t
move_can_go_on_left_or_right (asserv_position_t current_pos, 
			      asserv_position_t new_pos)
{
    // Go on right.
    if (new_pos.x > current_pos.x)
      {
	if (new_pos.x - current_pos.x < 3000 - MOVE_BORDER_END)
	    return 0x1;
	else
	    return 0x0;
      }
    // go on left.
    else 
      {
	if (current_pos.x - new_pos.x < MOVE_BORDER_END)
	    return 0x1;
	else
	    return 0x0;
      }
}

/** Go to the right.
  */
void
move_go_to_right (void)
{
    asserv_position_t pos;
    asserv_position_t new_pos;

    asserv_get_position (&pos);
    new_pos.x = pos.x - MOVE_BORDER_LEVEL;
    new_pos.y = pos.y;
    new_pos.a = pos.a;

    if (move_can_go_on_left_or_right (pos, new_pos))
      {
	asserv_goto (new_pos.x, new_pos.y);
      }
}

/** Go to the left.
  */
void
move_go_to_left (void)
{
    asserv_position_t pos;
    asserv_position_t new_pos;

    asserv_get_position (&pos);
    new_pos.x = pos.x + MOVE_BORDER_LEVEL;
    new_pos.y = pos.y;
    new_pos.a = pos.a;

    if (move_can_go_on_left_or_right (pos, new_pos))
      {
	asserv_goto (new_pos.x, new_pos.y);
      }
    
    return move_next (MOVE_ON_LEFT, failed);
}


