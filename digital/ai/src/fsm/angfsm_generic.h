/*
   AngFSM - Almost Non Generated Finite State Machine
   Copyright 2011-2013 Jerome Jutteau

 This file is part of AngFSM.

 AngFSM is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AngFSM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with AngFSM. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 You may not include this file in your code, this step is done by angfsm.h 
 */

#ifndef __GENERIC_ARCH__
#define __GENERIC_ARCH__

/* Disable init macro. */
#define ANGFSM_INIT

/** Disable options passing. */
#define ANGFSM_OPTIONS(argc, argv) (0)

/* Disable code generation macro. */
#define ANGFSM_GENERATE(arch, string)

/* Disable ANGFSM_IMPORT macro. */
#define ANGFSM_IMPORT(fsm_name)

/** Immediatly handle an event by the fsm. */
#define ANGFSM_HANDLE(fsm, event) angfsm_##fsm##_handle (angfsm_EVENT_##fsm##_##event)

/** Says if an event can actually be handle or not by the fsm. */
#define ANGFSM_CAN_HANDLE(fsm, event) angfsm_##fsm##_can_handle \
    (angfsm_EVENT_##fsm##_##event)

/** Reset fsm to it's initial state. */
#define ANGFSM_RESET(fsm) angfsm_##fsm##_init ();

/** Disable dot generation. */
#define ANGFSM_GEN_DOT(fsm, output)

/** Disable state defining */
#define ANGFSM_STATES(states...)

/** Disable event defining */
#define ANGFSM_EVENTS(events...)

/** Disable macro. */
#define ANGFSM_START_WITH(starters...)

#define ANGFSM_TRANS(state, event, output_branches...) \
    IFELSE_ARG1(void, ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_branch_t), output_branches) ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_trans_func_##state##_##event) ()

/** Transform an event in a numeric value. */
#define ANGFSM_EVENT(event) (ANGFSM_PASTE3_EXPAND (angfsm_EVENT_,ANGFSM_NAME,_##event))
#define ANGFSM_EVENT_F(fsm, event) angfsm_EVENT_##fsm##_##event

/** Transform a state in a numeric value. */
#define ANGFSM_STATE(state) (ANGFSM_PASTE3_EXPAND (angfsm_STATE_,ANGFSM_NAME,_##state))
#define ANGFSM_STATE_F(fsm, state) angfsm_STATE_##fsm##_##state

/** Transform a branch in a numeric value. */
#define ANGFSM_BRANCH(branch) (ANGFSM_PASTE3_EXPAND (angfsm_BRANCH_,ANGFSM_NAME,_##branch))
#define ANGFSM_BRANCH_F(fsm, branch) angfsm_BRANCH_##fsm##_##branch

/** Handle event from numeric event. */
#define ANGFSM_HANDLE_VAR(fsm, event) angfsm_##fsm##_handle (event)

/* Can we handle event from numeric event ? */
#define ANGFSM_CAN_HANDLE_VAR(fsm, event) angfsm_##fsm##_can_handle (event)

/* Time out macros. */
#define ANGFSM_TRANS_TIMEOUT(state, timeout, output_branches...) \
    ANGFSM_TRANS (state, state##_TIMEOUT, output_branches)

#define ANGFSM_HANDLE_TIMEOUT(fsm_name) angfsm_##fsm_name##_handle_timeout ()

/* Callback for transitions. */
#define ANGFSM_TRANS_CALLBACK(cb) \
    ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_trans_callback) = cb;

/* Get state string.
 * This macro is disabled if strings has no been embedded.
 */
#define ANGFSM_STATE_STR(state) \
    ((char *) ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_get_state_str (state)))

/* Get event string.
 * This macro is disabled if strings has no been embedded.
 */
#define ANGFSM_EVENT_STR(event) \
    ((char *) ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_get_event_str (event)))

/* Get branch string.
 * This macro is disabled if strings has no been embedded.
 */
#define ANGFSM_BRANCH_STR(branch) \
    ((char *) ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_get_branch_str (branch)))

#endif
