/* main.c */
/*  {{{
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
#include "../../fsm.h"
#include "../../playground.h"

#include <stdio.h>

int
main (void)
{
    fsm_init (&gutter_fsm);

    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_start);

    /* Move (angle here) command succeed */
    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_bot_move_succeed);

    /* Move (go to the wall) command succeed */
    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_bot_move_succeed);

    /* We have wait enough time for the all the balls to falls */
    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_wait_finished);

    return 0;
}

