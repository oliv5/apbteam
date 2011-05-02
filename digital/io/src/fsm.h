/*
   AngFSM - AngFSM - Almost Non Generated Finite State Machine

   Copyright 2011 Jerome Jutteau

Contact:
 * email: j.jutteau _AT_ gmail _DOT_ com
 * website: http://fuu.im/angfsm/

 This file is part of AngFSM.

 AngFSM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AngFSM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with AngFSM. If not, see <http://www.gnu.org/licenses/>.
 */

#include <inttypes.h>

#ifndef _FSM_GEN_
#define _FSM_GEN_

#ifndef FSM_NAME
#define FSM_NAME FSM
#endif

#define XSTR(s) STR(s)
#define STR(s) #s
#define FSM_PASTE(a, b) a ## b
#define FSM_PASTE_EXPAND(a, b) FSM_PASTE (a, b)
#define FSM_PASTE3_EXPAND(a, b, c)\
    FSM_PASTE_EXPAND (a, FSM_PASTE_EXPAND (b, c))
#define FIRST(first, others...) first

#ifdef __AVR_ARCH__
/* Include generated header. */
#include XSTR (FSM_PASTE_EXPAND (fsm_, FSM_NAME)_gen.h)

/* Disable init macro. */
#define FSM_INIT

/* Disable code generation macro. */
#define FSM_GENERATE(arch, string)

/* Disable FSM_IMPORT macro. */
#define FSM_IMPORT(fsm_name)

/** Immediatly handle an event by the fsm. */
#define FSM_HANDLE(fsm, event) fsm_##fsm##_handle (FSM_EVENT_##fsm##_##event )

/** Says if an event can actually be handle or not by the fsm. */
#define FSM_CAN_HANDLE(fsm, event) fsm_##fsm##_can_handle \
    (FSM_EVENT_##fsm##_##event)

/** Reset fsm to it's initial state. */
#define FSM_RESET(fsm) fsm_##fsm##_init ();

/** Disable dot generation. */
#define FSM_GEN_DOT(fsm)

/** Disable state defining
 *
 * Define states of the fsm, can be called several times to add other states. */
#define FSM_STATES(states...)

/** Disable event defining */
#define FSM_EVENTS(events...)

/** Disable macro. */
#define FSM_START_WITH(starters...)

#define FSM_TRANS(state, event, output_branches...)\
    FSM_PASTE3_EXPAND (fsm_, FSM_NAME,_branch_t)\
FSM_PASTE3_EXPAND (fsm_, FSM_NAME,_trans_func_##state##_##event) ()

#define FSM_NEXT(state, event, branch...)\
    FSM_PASTE3_EXPAND (FSM_BRANCH_, FSM_NAME,_##state##_##event##_##branch)

/** Transform an event in uint16_t. */
#define FSM_EVENT(fsm, event) FSM_EVENT_##fsm##_##event

/** Handle event from uint16_t. */
#define FSM_HANDLE_VAR(fsm, event) fsm_##fsm##_handle (event)

/* Can we handle event from uint16_t ? */
#define FSM_CAN_HANDLE_VAR(fsm, event) fsm_##fsm##_can_handle (event)

/* Time out macros. */
#define FSM_TRANS_TIMEOUT(state, timeout, output_branches...)\
    FSM_TRANS (state, state##_TIMEOUT, output_branches)

#define FSM_NEXT_TIMEOUT(state, branch...)\
    FSM_NEXT (state, state##_TIMEOUT, branch)

#define FSM_HANDLE_TIMEOUT(fsm_name) fsm_##fsm_name##_handle_timeout ()

#else /* Compiling for HOST */
#include <stdlib.h>

/* Describe an event. */
typedef struct
{
    /* Name of the event. */
    char *var_name;
    /* Unique code of the event. */
    uint code;
} fsm_build_event_t;

/* Chain of events. */
typedef struct fsm_build_event_chain_t
{
    fsm_build_event_t event;
    struct fsm_build_event_chain_t *next;
} fsm_build_event_chain_t;

/* Describe a state. Same as event. */
typedef fsm_build_event_t fsm_build_state_t;

/* Chain of state */
typedef struct fsm_build_state_chain_t
{
    fsm_build_state_t state;
    struct fsm_build_state_chain_t *next;
} fsm_build_state_chain_t;

/**
 * Describe a branch. It is the output of a transition.
 * Branches can be chained when there are several possible output state for
 * one event.
 */
typedef struct fsm_build_branch_chain_t
{
    /* Pointer to the output state of the branch. */
    fsm_build_state_t *state;
    /* Name of the branch. */
    char *name;
    /* Pointer to the next branch when we have the choice of the output */
    struct fsm_build_branch_chain_t *next;
} fsm_build_branch_chain_t;

/**
 * Describe a transition.
 * When an event occurs, an active state may react to this event an execute a
 * transition. The transition function returns the new state which become
 * active.
 */
typedef struct
{
    /* Active state. */
    fsm_build_state_t *state;
    /* Active event. */
    fsm_build_event_t *event;
    /* Possible states the transition will return. */
    fsm_build_branch_chain_t *output_branches;
} fsm_build_trans_t;

/* Chain of transitions. */
typedef struct fsm_build_trans_chain_t
{
    fsm_build_trans_t trans;
    struct fsm_build_trans_chain_t *next;
} fsm_build_trans_chain_t;

/* Pointer to a transition function. */
typedef fsm_build_state_t* (*fsm_build_run_strans_func_t)(void);

/* Chain of transitions with associated function's pointer. */
typedef struct fsm_trans_func_chain_t
{
    fsm_build_run_strans_func_t func;
    fsm_build_trans_t trans;
    struct fsm_trans_func_chain_t *next;
} fsm_trans_func_chain_t;

/* Timeout structure. */
typedef struct fsm_build_timeout_t
{
    uint timeout;
    fsm_build_trans_t *trans;
} fsm_build_timeout_t;

/* Chain of fsm_timeout_t. */
typedef struct fsm_build_timeout_chain_t
{
    fsm_build_timeout_t timeout;
    struct fsm_build_timeout_chain_t *next;
} fsm_build_timeout_chain_t;

/* Data needed for fsm execution (for host). */
typedef struct
{
    /* This table store pointers of aff all transition functions. The first
     * dimension correspond to the events and the second to the states. If
     * there is no transition of a event/state couple, NULL is set. This
     * permit to react quickly to an event during execution but is not the
     * best for memory cost.
     */
    fsm_build_run_strans_func_t **trans_table;
    /* Store pointer to active states. */
    fsm_build_state_t **active_states;
    /* This array has the size of active_states and store the event who has
     * triggered the corresponding active state. This is used for dot
     * generation for visual purposes.
     */
    fsm_build_event_t **events_before_active_state;
    /* Store all pointers of transition functions. */
    fsm_trans_func_chain_t *func_pool;
    /* Array of counters for timeout events.
     * -1 mean counter is off. */
    int *timeout_counters;
} fsm_build_run_t;

/* Store all Finite State Machine (fsm) informations. */
typedef struct
{
    /* All events. */
    fsm_build_event_chain_t *events;
    /* All states. */
    fsm_build_state_chain_t *states;
    /* All transitions. */
    fsm_build_trans_chain_t *trans;
    /* Name of the fsm. */
    char *name;
    /* Maximal number of active states. */
    uint max_active_states;
    /* Total number of events. */
    uint event_nb;
    /* Total number of states. */
    uint state_nb;
    /* First active states. */
    fsm_build_state_chain_t *starters;
    /* All timeout. */
    fsm_build_timeout_chain_t *timeouts;
    /* Data for running purposes. */
    fsm_build_run_t run;
} fsm_build_t;

/* Store all fsm in a chain. */
typedef struct fsm_build_chain_t
{
    fsm_build_t *fsm;
    struct fsm_build_chain_t *next;
} fsm_build_chain_t;

/* Store all fsm. */
extern fsm_build_chain_t *fsm_build_all_fsm;
/* Function to initialize fsm_build_all_fsm. */
void fsm_build_init_all_fsm () __attribute__((constructor(101)));

/** Create the fsm and functions who will be executed before and after main
 * function. This permits initilialization of the fsm. */
extern fsm_build_t FSM_PASTE_EXPAND (fsm_, FSM_NAME);
void FSM_PASTE_EXPAND (fsm_build_init_, FSM_NAME)() __attribute__((constructor(102)));
void FSM_PASTE_EXPAND (fsm_build_run_init_, FSM_NAME)() __attribute__((constructor(107)));
void FSM_PASTE_EXPAND (fsm_build_free_, FSM_NAME)() __attribute__((destructor));

#define FSM_INIT\
    fsm_build_t FSM_PASTE_EXPAND (fsm_, FSM_NAME);\
void FSM_PASTE_EXPAND (fsm_build_init_, FSM_NAME)()\
{\
    fsm_build_init (FSM_PASTE_EXPAND (&fsm_, FSM_NAME), XSTR(FSM_NAME));\
}\
void FSM_PASTE_EXPAND (fsm_build_run_init_, FSM_NAME)()\
{\
    fsm_build_run_init (& FSM_PASTE_EXPAND (fsm_, FSM_NAME));\
}\
void FSM_PASTE_EXPAND (fsm_build_free_, FSM_NAME)()\
{\
    fsm_build_free (& FSM_PASTE_EXPAND (fsm_, FSM_NAME));\
}

#define FSM_IMPORT(fsm_name)\
    extern fsm_build_t fsm_##fsm_name;

/** Generate source ou header files for the specific architecture */
#define FSM_GENERATE(arch, string) fsm_build_gen (#arch, string);

/** Immediatly handle an event by the fsm. */
#define FSM_HANDLE(fsm, event) fsm_build_handle_string (&fsm_##fsm , #event)

/** Says if an event can actually be handle or not by the fsm. */
#define FSM_CAN_HANDLE(fsm, event) fsm_build_can_handle_string (&fsm_##fsm, #event)

/** Reset fsm to it's initial state. */
#define FSM_RESET(fsm) fsm_build_reset (&fsm_##fsm)

/** Generate dot. */
#define FSM_GEN_DOT(fsm) fsm_build_gen_dot (&fsm_##fsm)

/** Define states of the fsm, can be called several times to add other states. */
#define FSM_STATES(states...)\
    void FSM_PASTE3_EXPAND (fsm_build_states_, FSM_NAME, FIRST (states)) () __attribute__((constructor(103)));\
void FSM_PASTE3_EXPAND (fsm_build_states_, FSM_NAME, FIRST (states)) ()\
{\
    fsm_build_states (& FSM_PASTE_EXPAND (fsm_, FSM_NAME), #states);\
}

/** Define events of the fsm, can be called several times to add other events. */
#define FSM_EVENTS(events...)\
    void FSM_PASTE3_EXPAND (fsm_build_events_, FSM_NAME, FIRST (events)) () __attribute__((constructor(103)));\
void FSM_PASTE3_EXPAND (fsm_build_events_, FSM_NAME, FIRST (events)) ()\
{\
    fsm_build_events (& FSM_PASTE_EXPAND (fsm_, FSM_NAME), #events);\
}

/** Define wich state(s) will be active at the beginning. */
#define FSM_START_WITH(starters...)\
    void FSM_PASTE3_EXPAND (fsm_build_start_with_, FSM_NAME, FIRST (starters))() __attribute__((constructor(104)));\
void FSM_PASTE3_EXPAND (fsm_build_start_with_, FSM_NAME, FIRST (starters))()\
{\
    fsm_build_start_with (& FSM_PASTE_EXPAND (fsm_, FSM_NAME), #starters);\
}

/**
 * Define a transition by giving a state, an event and:
 * - a event if the transition returns only one state.
 * - branches if the transition returns different states.
 * See examples for reel usage.
 */
#define FSM_TRANS(state, event, output_branches...)\
    fsm_build_state_t* FSM_PASTE3_EXPAND (fsm_trans_, FSM_NAME,_##state##_##event) ();\
void FSM_PASTE3_EXPAND (fsm_build_trans_, FSM_NAME,_##state##_##event)() __attribute__((constructor(105)));\
void FSM_PASTE3_EXPAND (fsm_build_trans_, FSM_NAME,_##state##_##event)()\
{\
    fsm_build_trans (& FSM_PASTE_EXPAND(fsm_, FSM_NAME), #state, #event,\
#output_branches,\
		     & FSM_PASTE3_EXPAND (fsm_trans_, FSM_NAME,_##state##_##event));\
}\
fsm_build_state_t* FSM_PASTE3_EXPAND (fsm_trans_, FSM_NAME,_##state##_##event) ()

/**
 * Used to return next state by giving the actual transition informations and
 * the branch (if there are several branches).
 * Not directly returning the state can avoid some errors. *
 */
#define FSM_NEXT(state, event, branch...)\
    fsm_build_get_next_state (& FSM_PASTE_EXPAND (fsm_, FSM_NAME), #state, #event, #branch)

/**
 * Define a transition when a state times out.
 * You have to provide a state, a timeout value and:
 * - a event if the transition returns only one state.
 * - branches if the transition returns different states.
 * See examples for reel usage.
 */
#define FSM_TRANS_TIMEOUT(state, timeout, output_branches...)\
    void FSM_PASTE3_EXPAND (fsm_build_timeout_, FSM_NAME,_##state##_TIMEOUT)() __attribute__((constructor(106)));\
void FSM_PASTE3_EXPAND (fsm_build_timeout_, FSM_NAME,_##state##_TIMEOUT)()\
{\
    fsm_build_timeout (& FSM_PASTE_EXPAND(fsm_, FSM_NAME), #state, XSTR(state##_TIMEOUT),  timeout);\
}\
FSM_EVENTS (state##_TIMEOUT)\
FSM_TRANS (state, state##_TIMEOUT, output_branches)

/**
 * Used to return next state after a timeout.
 * Same as FSM_NEXT but without specifying an event.
 */
#define FSM_NEXT_TIMEOUT(state, branch...)\
    FSM_NEXT (state, state##_TIMEOUT, branch)

/** Used to handle timeout events. */
#define FSM_HANDLE_TIMEOUT(fsm_name) fsm_build_handle_timeout (& FSM_PASTE_EXPAND (fsm_, FSM_NAME))

/** Transform an event in uint16_t. */
#define FSM_EVENT(fsm_name, event) fsm_build_get_event_code (& FSM_PASTE_EXPAND (fsm_, FSM_NAME), #event)

/** Handle event from uint16_t. */
#define FSM_HANDLE_VAR(fsm, event) fsm_build_handle_integer (& FSM_PASTE_EXPAND (fsm_, FSM_NAME), event)

/* Can we handle event from uint16_t ? */
#define FSM_CAN_HANDLE_VAR(fSM, event) fsm_build_can_handle_integer (& FSM_PASTE_EXPAND (fsm_, FSM_NAME), event)

/**
 * Parse a string who contains a list of arguments seperated by comma.
 * It will create and store each parameters (without space and ") in an array
 * and give the number of parsed arguments.
 * \param  string  string to parse
 * \param  tab  pointer to a table of strings where to store the array
 * \param  nb  pointer where to store the number of parsed arguments
 */
void
fsm_build_arg_parse (char *string, char ***tab, int *nb);

/**
 * Free an array of strings generated by fsm_build_arg_parse.
 * \param  tab  pointer to the array of strings
 * \param  nb  number of arguments stored in the array.
 */
void
fsm_build_arg_free (char ***tab, int nb);

/**
 * This function is executed when a state is returned by a transition.
 * \param  fsm  fsm
 * \param  trans transition where the return occurs
 * \param  branch  branch to transition has chosen.
 */
void
fsm_build_print (fsm_build_t *fsm,
		 fsm_build_trans_t* trans,
		 fsm_build_branch_chain_t* branch);

/**
 * Test the fsm and search for errors.
 */
void
fsm_build_sanity_check (fsm_build_t *fsm);

/** Reset the fsm to it's initial state. */
void
fsm_build_reset (fsm_build_t *fsm);

/** Generate the dot file of the actual fsm state. */
void
fsm_build_gen_dot (fsm_build_t *fsm);

/** Initialize the fsm. */
void
fsm_build_init (fsm_build_t *fsm, char *name);

/** Initialize the running data of the fsm. */
void
fsm_build_run_init (fsm_build_t *fsm);

/**
 * Add some states to the fsm.
 * \param  fsm  fsm
 * \param  states  states separated by comma
 **/
void
fsm_build_states (fsm_build_t *fsm, char *states);

/**
 * Add some events to the fsm.
 * \param  fsm  fsm
 * \param  events  events separated by comma
 **/
void
fsm_build_events (fsm_build_t *fsm, char *events);

/** Get event's pointer by giving it's name. */
fsm_build_event_t*
fsm_build_get_event (fsm_build_t *fsm, char *event);

/** Get state's pointer by giving it's name. */
fsm_build_state_t*
fsm_build_get_state (fsm_build_t *fsm, char *state);

/** Get event's pointer by giving it's code. */
fsm_build_event_t*
fsm_build_get_event_by_code (fsm_build_t *fsm, uint event);

/** Get state's pointer by giving it's code. */
fsm_build_state_t*
fsm_build_get_state_by_code (fsm_build_t *fsm, uint state);

/** Get event code as uint16_t */
uint16_t
fsm_build_get_event_code (fsm_build_t *fsm, char *event);

/**
 * Add a transition to the fsm.
 * \param  fsm  fsm
 * \param  state  state who will be active for the transition
 * \param  event  event to react
 * \param  output_branches  list of output branches seperated by comma or a
 * single state (if there is only one branche).
 * For example:
 * - If the transition return a single state, it will be "stateX"
 * - If the transition can return different states, it will be
 *   "branch1, state1, branch2, state2, branch3, state3" [...]
 * \param  trans_func  pointer to the transition function.
 */
void
fsm_build_trans (fsm_build_t *fsm,
		 char *state,
		 char *event,
		 char *output_branches,
		 fsm_build_run_strans_func_t trans_func);

/* Create transition based on a timeout.
 * \param  fsm  fsm
 * \param  state  state who times out
 * \param  event  name of the timeout (specific to this state and described in macro)
 * \param  timeout  value of timeout
 */
void
fsm_build_timeout (fsm_build_t *fsm, char *state, char *event, uint timeout);

/**
 * Define with which states the fsm will start.
 * \param  fsm  fsm
 * \param  starters  list of states seperated by comma
 */
void
fsm_build_start_with (fsm_build_t *fsm, char *starters);

/** Handle an event by the fsm.
 * \param  fsm  fsm
 * \param  event event to handle with fsm
 * \return  zero if event is not handled at all, one otherwise
 **/
int
fsm_build_handle (fsm_build_t *fsm, fsm_build_event_t *event);
int
fsm_build_handle_string (fsm_build_t *fsm, char *event);
int
fsm_build_handle_integer (fsm_build_t *fsm, uint16_t event);

/**
 * Say if the event can be handled or not.
 * \param  fsm  fsm
 * \param  event  event to test with the fsm
 * \return  zero if the event can be handled, non-zero otherwhise
 */
int
fsm_build_can_handle (fsm_build_t *fsm, fsm_build_event_t *event);
int
fsm_build_can_handle_string (fsm_build_t *fsm, char *event);
int
fsm_build_can_handle_integer (fsm_build_t *fsm, uint16_t event);

/**
 * Handle timeout events of the fsm.
 * \param  fsm  fsm
 * \return  one if an event has been handled, zero otherwise
 */
int
fsm_build_handle_timeout (fsm_build_t *fsm);

/** Give the state at the transition output. */
fsm_build_state_t*
fsm_build_get_next_state (fsm_build_t *fsm,
			  char *state,
			  char *event,
			  char *branch);

/** Generate header file for target which provides more optimised version of
 * fsm execution
 * \param  arch  specify generated architecture
 * \param  embedded_strings  ask to embed states and events strings (0 = not
 * embedded)
 */
void
fsm_build_gen (char *arch, uint embedded_strings);
void
fsm_build_gen_avr_h (fsm_build_t *fsm, uint embedded_strings);
void
fsm_build_gen_avr_c (fsm_build_t *fsm, uint embedded_strings);

/** Free fsm allocated data. */
void
fsm_build_free (fsm_build_t *fsm);

#endif /* Architectures */

#endif /* #ifdef _FSM_GEN_ */
