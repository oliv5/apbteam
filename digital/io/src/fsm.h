#ifndef fsm_h
#define fsm_h
/* fsm.h - Finite State Machine code. */
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

/**
 * The io board includes several concurrent FSM.  The code to handle events is
 * generic, but as events are not compatibles (they do not share the same
 * identifier), events should be generated for each FSM separately.
 *
 * The main loop tests for each possible event and generate the corresponding
 * FSM events.  For example:
 *
 *   if (asserv_move_cmd_status () == success)
 *     {
 *       fsm_handle_event (&top_fsm, TOP_EVENT_position_reached);
 *       fsm_handle_event (&getsamples_fsm, GETSAMPLES_EVENT_position_reached);
 *     }
 *
 * Any unhandled event will be ignored.
 *
 * To start a sub machine from the top FSM, call the sub machine start
 * function (for example getsamples_start), which will set parameters, reset
 * its fsm, then trigger its start event.
 *
 * To abandon a FSM, reset it using fsm_init or it will continue to run
 * concurrently.
 *
 * The sub machine is expected to generate an event for the top machine to
 * make it continue.
 */

#ifdef HOST
/** On host, check branches. */
typedef u32 fsm_branch_t;
#else
/** On AVR, no check. */
typedef u8 fsm_branch_t;
#endif

/** Transition function. */
typedef fsm_branch_t (*fsm_transition_t) (void);

/** Context of a FSM or sub-FSM. */
struct fsm_t
{
    /** Transition table. */
    const fsm_transition_t *transition_table;
    /** Number of events, used to index the right transition. */
    u8 events_nb;
    /** Initial state. */
    u8 state_init;
    /** Current active state. */
    u8 state_current;
#ifdef HOST
    /** FSM name. */
    const char *name;
    /** States names. */
    const char * const *states_names;
    /** Events names. */
    const char * const *events_names;
#endif
};
typedef struct fsm_t fsm_t;

/** Reset a FSM. */
void
fsm_init (fsm_t *fsm);

/** Handle an event on the given FSM.
 * @return
 *   - 0 if this FSM does not handle this event ;
 *   - 1 if this FSM handles this event in its current state.
 */
uint8_t
fsm_handle_event (fsm_t *fsm, u8 event);

#include "getsamples_fsm.h"
#include "gutter_fsm.h"
#include "move_fsm.h"
#include "top_fsm.h"
#include "filterbridge_fsm.h"
#include "elevator_fsm.h"

#endif /* fsm_h */
