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

#ifdef HOST
# include <stdio.h>
#endif

/** Reset a FSM. */
void
fsm_init (fsm_t *fsm)
{
    assert (fsm);
    fsm->state_current = fsm->state_init;
}

/** Handle an event on the given FSM. */
void
fsm_handle_event (fsm_t *fsm, u8 event)
{
    assert (fsm);
    assert (event < fsm->events_nb);
    /* Lookup transition. */
    fsm_transition_t tr = fsm->transition_table[
	fsm->state_current * fsm->events_nb + event];
    /* Ignore unhandled events. */
    if (tr)
      {
	/* Execute transition. */
	fsm_branch_t br = tr ();
	/* Change state. */
#ifdef HOST
	assert (((br >> 16) & 0xff) == fsm->state_current);
	assert (((br >> 8) & 0xff) == event);
	printf ("%s %s =%s=> %s\n", fsm->name,
		fsm->states_names[fsm->state_current],
		fsm->events_names[event], fsm->states_names[br & 0xff]);
	fsm->state_current = br & 0xff;
#else
	fsm->state_current = br;
#endif
      }
}

