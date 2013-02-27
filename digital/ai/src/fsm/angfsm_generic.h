/*
   AngFSM - Almost Non Generated Finite State Machine
   Copyright 2011, 2012 Jerome Jutteau

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

/** Disable state defining
 *
 * Define states of the fsm, can be called several times to add other states. */
#define ANGFSM_STATES(states...)

/** Disable event defining */
#define ANGFSM_EVENTS(events...)

/** Disable macro. */
#define ANGFSM_START_WITH(starters...)

#define ANGFSM_TRANS(state, event, output_branches...) \
    ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_branch_t) \
    ANGFSM_PASTE3_EXPAND (angfsm_, ANGFSM_NAME,_trans_func_##state##_##event) ()

#define ANGFSM_NEXT(state, event, branch...) \
    ANGFSM_PASTE3_EXPAND (angfsm_BRANCH_, ANGFSM_NAME,_##state##_##event##_##branch)

/** Transform an event in uint16_t. */
#define ANGFSM_EVENT(fsm, event) angfsm_EVENT_##fsm##_##event

/** Handle event from uint16_t. */
#define ANGFSM_HANDLE_VAR(fsm, event) angfsm_##fsm##_handle (event)

/* Can we handle event from uint16_t ? */
#define ANGFSM_CAN_HANDLE_VAR(fsm, event) angfsm_##fsm##_can_handle (event)

/* Time out macros. */
#define ANGFSM_TRANS_TIMEOUT(state, timeout, output_branches...) \
    ANGFSM_TRANS (state, state##_TIMEOUT, output_branches)

#define ANGFSM_NEXT_TIMEOUT(state, branch...) \
    ANGFSM_NEXT (state, state##_TIMEOUT, branch)

#define ANGFSM_HANDLE_TIMEOUT(fsm_name) angfsm_##fsm_name##_handle_timeout ()

#endif