/* robot_main_fsm.c */
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
#include "topfsm.h"
#include "fsm.h"

struct topfsm_data_t topfsm_data;

/** Start a main FSM. */
void
robot_main_start (void)
{
    /* Set parameters. */
    topfsm_data.sequence = 0x15;
    /* Start the FSM. */
/*    fsm_init (&robot_main_fsm);
    fsm_handle_event (&robot_main_fsm, ROBOT_MAIN_EVENT_ok);
    */
}

