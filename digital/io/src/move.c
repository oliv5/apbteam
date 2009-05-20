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

#include "main.h"

/**
 * Internal data used by the move FSM.
 */
struct move_data_t move_data;

void
move_start (asserv_position_t position, uint8_t backward)
{
    /* Set parameters. */
    move_data.final.x = position.x;
    move_data.final.y = position.y;
    move_data.final.a = position.a;
    move_data.backward_movement_allowed = backward;
    /* XXX */
    main_always_stop_for_obstacle = 1;
    /* Reset move FSM flags */
    main_sharp_ignore_event = 0;
    main_move_wait_cycle = 0;
    /* Start the FSM. */
    fsm_init (&move_fsm);
    fsm_handle_event (&move_fsm, MOVE_EVENT_start);
}

void
move_stop (void)
{
    /* This is a dirty hack. */
    /* Re-init move FSM. */
    fsm_init (&move_fsm);
    /* Ask the asserv to stop the motor. */
    asserv_stop_motor ();
    /* Generate the event, move FSM stopped. */
    main_post_event_for_top_fsm = TOP_EVENT_move_fsm_stopped;
}
