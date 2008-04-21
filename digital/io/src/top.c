/* top.c */
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
#include "top.h"
#include "fsm.h"

struct top_data_t top_data;

/**
 * When we need to tell the main loop we want to be alerted when the last
 * command sent to the asserv board has been acknowledged.
 */
uint8_t top_waiting_for_settings_ack_ = 0;

/** Start a main FSM. */
void
top_start (void)
{
    /* Set parameters. */
    top_data.sequence = 0x15;
    top_data.boxes_used = 0x0;
    top_data.sequence_to_do = 0x15;
    /* Start the FSM. */
    fsm_init (&top_fsm);
    fsm_handle_event (&top_fsm, TOP_EVENT_start);
}

/* Do we need to tell the top FSM the settings has been acknowledged? */
uint8_t
top_generate_settings_ack_event (void)
{
    return top_waiting_for_settings_ack_;
}
