/* gutter.c */
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
#include "gutter.h"
#include "fsm.h"

/* Number of cycles to wait. */
extern uint16_t gutter_wait_cycle_;

/* Start the gutter FSM. */
void
gutter_start (void)
{
    /* Start the FSM. */
    fsm_init (&gutter_fsm);
    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_start);
}

/* Do we need to generate a wait_finished event? */
uint8_t
gutter_generate_wait_finished_event (void)
{
    if (gutter_wait_cycle_)
	return !(--gutter_wait_cycle_);
    else
	return 0;
}
