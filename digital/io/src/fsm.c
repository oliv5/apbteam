/* fsm.c - Finite State Machine code. */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "fsm.h"

#include "modules/trace/trace.h"
#include "trace_event.h"

#ifdef HOST
# include <stdio.h>
#endif

void
fsm_init (fsm_t *fsm)
{
    uint8_t i;
    assert (fsm);
    for (i = 0; i < FSM_ACTIVE_STATES_MAX; i++)
	fsm->states_active[i] = fsm->states_init[i];
    for (i = 0; i < fsm->active_states_nb; i++)
	fsm->states_timeout[i] =
	    fsm->state_timeout_table[fsm->states_init[i]];
}

/** Handle an event on the given FSM on a single active state, return 1 if the
 * event was handled. */
static uint8_t
fsm_handle_event_single (fsm_t *fsm, uint8_t active, uint8_t event)
{
    assert (fsm);
    assert (active < fsm->active_states_nb);
    assert (event < fsm->events_nb);
    /* Lookup transition. */
    uint8_t old_state = fsm->states_active[active];
    fsm_transition_t tr = fsm->transition_table[
	old_state * fsm->events_nb + event];
    /* Ignore unhandled events. */
    if (tr)
      {
	/* Execute transition. */
	uint8_t new_state;
	fsm_branch_t br = tr ();
	/* Change state. */
#ifdef HOST
	assert (((br >> 16) & 0xff) == fsm->states_active[active]);
	assert (((br >> 8) & 0xff) == event);
	fprintf (stderr, "%s %s =%s=> %s\n", fsm->name,
		fsm->states_names[fsm->states_active[active]],
		fsm->events_names[event], fsm->states_names[br & 0xff]);
	new_state = br & 0xff;
#else
	new_state = br;
#endif
	TRACE (TRACE_FSM__HANDLE_EVENT, (u8) old_state, (u8) event,
	       (u8) new_state);
	fsm->states_active[active] = new_state;
	fsm->states_timeout[active] = fsm->state_timeout_table[new_state];
	return 1;
      }
    return 0;
}

uint8_t
fsm_handle_event (fsm_t *fsm, uint8_t event)
{
    uint8_t i, handled = 0;
    assert (fsm);
    assert (event < fsm->events_nb);
    for (i = 0; i < fsm->active_states_nb; i++)
      {
	/* Handle the event for this active state. */
	handled += fsm_handle_event_single (fsm, i, event);
      }
    return handled;
}

uint8_t
fsm_handle_timeout (fsm_t *fsm)
{
    uint8_t i, handled = 0;
    assert (fsm);
    for (i = 0; i < fsm->active_states_nb; i++)
      {
	/* If there is a timeout for this state. */
	if (fsm->states_timeout[i] != 0xffff)
	  {
	    if (fsm->states_timeout[i])
		fsm->states_timeout[i]--;
	    else
	      {
		/* Timeout expired, generate corresponding event. */
		handled += fsm_handle_event_single (fsm, i,
						    fsm->state_timeout_event);
	      }
	  }
      }
    return handled;
}

