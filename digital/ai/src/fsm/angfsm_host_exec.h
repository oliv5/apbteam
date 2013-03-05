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

#ifndef __HOST_EXEC__
#define __HOST_EXEC__

#include <stdlib.h>
#include <stdio.h>

typedef enum {
    ANGFSM_BUILD_ARCH_HOST,
    ANGFSM_BUILD_ARCH_AVR,
    ANGFSM_BUILD_ARCH_ARM,
    ANGFSM_BUILD_ARCH_NB,
} angfsm_build_arch_t;

extern const char *angfsm_build_arch_name[ANGFSM_BUILD_ARCH_NB];

typedef struct {
    int embedded_strings;
    int sanity_check;
    char *gen_dot;
    char *gen_code;
    int print_trans;
} angfsm_build_user_options_t;

/* Describe an event. */
typedef struct {
    /* Name of the event. */
    char *var_name;
    /* Unique code of the event. */
    uint code;
} angfsm_build_event_t;

/* Chain of events. */
typedef struct angfsm_build_event_chain_t {
    angfsm_build_event_t event;
    struct angfsm_build_event_chain_t *next;
} angfsm_build_event_chain_t;

/* Describe a state. Same as event. */
typedef angfsm_build_event_t angfsm_build_state_t;

/* Chain of state */
typedef struct angfsm_build_state_chain_t {
    angfsm_build_state_t state;
    struct angfsm_build_state_chain_t *next;
} angfsm_build_state_chain_t;

/**
 * Describe a branch. It is the output of a transition.
 * Branches can be chained when there are several possible output state for
 * one event.
 */
typedef struct angfsm_build_branch_chain_t {
    /* Pointer to the output state of the branch. */
    angfsm_build_state_t *state;
    /* Name of the branch. */
    char *name;
    /* Pointer to the next branch when we have the choice of the output */
    struct angfsm_build_branch_chain_t *next;
} angfsm_build_branch_chain_t;

/**
 * Describe a transition.
 * When an event occurs, an active state may react to this event an execute a
 * transition. The transition function returns the new state which become
 * active.
 */
typedef struct {
    /* Active state. */
    angfsm_build_state_t *state;
    /* Active event. */
    angfsm_build_event_t *event;
    /* Possible states the transition will return. */
    angfsm_build_branch_chain_t *output_branches;
} angfsm_build_trans_t;

/* Chain of transitions. */
typedef struct angfsm_build_trans_chain_t {
    angfsm_build_trans_t trans;
    struct angfsm_build_trans_chain_t *next;
} angfsm_build_trans_chain_t;

/* Pointer to a transition function.
 * out_branch corresponds to function's return.
 * Mono branch transitions do not return anything,
 * It permits to avoid ugly warnings.
 * Multi branches returns a uint, the function is transtyped as a
 * angfsm_build_run_strans_func_branches_t
 */
typedef void (*angfsm_build_run_strans_func_t) ();
typedef uint (*angfsm_build_run_strans_func_branches_t) ();

/* Chain of transitions with associated function's pointer. */
typedef struct angfsm_trans_func_chain_t {
    angfsm_build_run_strans_func_t func;
    angfsm_build_trans_t trans;
    struct angfsm_trans_func_chain_t *next;
} angfsm_trans_func_chain_t;

/* Timeout structure. */
typedef struct angfsm_build_timeout_t {
    uint timeout;
    angfsm_build_trans_t *trans;
} angfsm_build_timeout_t;

/* Chain of angfsm_timeout_t. */
typedef struct angfsm_build_timeout_chain_t {
    angfsm_build_timeout_t timeout;
    struct angfsm_build_timeout_chain_t *next;
} angfsm_build_timeout_chain_t;

/* Data needed for fsm execution (for host). */
typedef struct {
    /* This table store pointers of aff all transition functions. The first
     * dimension correspond to the events and the second to the states. If
     * there is no transition of a event/state couple, NULL is set. This
     * permit to react quickly to an event during execution but is not the
     * best for memory cost.
     */
    angfsm_build_run_strans_func_t **trans_table;
    /* Store pointer to active states. */
    angfsm_build_state_t **active_states;
    /* This array has the size of active_states and store the event who has
     * triggered the corresponding active state. This is used for dot
     * generation for visual purposes.
     */
    angfsm_build_event_t **events_before_active_state;
    /* Store all pointers of transition functions. */
    angfsm_trans_func_chain_t *func_pool;
    /* Array of counters for timeout events.
     * -1 mean counter is off. */
    int *timeout_counters;
    /* Callback to transitions. */
    void (*trans_callback) (int state,
                            int event,
                            int output_branch,
                            int branch);
} angfsm_build_run_t;

/* Store all Finite State Machine (fsm) informations. */
typedef struct {
    /* All events. */
    angfsm_build_event_chain_t *events;
    /* All states. */
    angfsm_build_state_chain_t *states;
    /* All transitions. */
    angfsm_build_trans_chain_t *trans;
    /* Name of the fsm. */
    char *name;
    /* Maximal number of active states. */
    uint max_active_states;
    /* Maximal number of events per states. */
    uint max_events_per_states;
    /* Maximal number of branches per transitions. */
    uint max_branches_per_trans;
    /* Total number of events. */
    uint event_nb;
    /* Total number of states. */
    uint state_nb;
    /* First active states. */
    angfsm_build_state_chain_t *starters;
    /* All timeout. */
    angfsm_build_timeout_chain_t *timeouts;
    /* Total number of unique branch name. */
    uint u_branch_nb;
    /* List of unique branches strings.
     * Unique number corresponding to string is the position of the string
     *in the array.
     */
    char **u_branch_name;
    /* Data for running purposes. */
    angfsm_build_run_t run;
    /* User's options. */
    angfsm_build_user_options_t options;
} angfsm_build_t;

/* Store all fsm in a chain. */
typedef struct angfsm_build_chain_t {
    angfsm_build_t *fsm;
    struct angfsm_build_chain_t *next;
} angfsm_build_chain_t;

/* Store all fsm. */
extern angfsm_build_chain_t *fsm_build_all_fsm;
/* Function to initialize angfsm_build_all_fsm. */
void angfsm_build_init_all_fsm() __attribute__((constructor(101)));

/** Create the fsm and functions who will be executed before and after main
 * function. This permits initilialization of the fsm. */
extern angfsm_build_t ANGFSM_PASTE_EXPAND(angfsm_, ANGFSM_NAME);
void ANGFSM_PASTE_EXPAND(angfsm_build_init_, ANGFSM_NAME)() __attribute__((constructor(102)));
void ANGFSM_PASTE_EXPAND(angfsm_build_init_finalize_, ANGFSM_NAME)() __attribute__((constructor(107)));
void ANGFSM_PASTE_EXPAND(angfsm_build_run_init_, ANGFSM_NAME)() __attribute__((constructor(108)));
void ANGFSM_PASTE_EXPAND(angfsm_build_free_, ANGFSM_NAME)() __attribute__((destructor));

#define ANGFSM_INIT \
    angfsm_build_t ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME); \
    void ANGFSM_PASTE_EXPAND (angfsm_build_init_, ANGFSM_NAME)() \
    { \
        angfsm_build_init (ANGFSM_PASTE_EXPAND (&angfsm_, ANGFSM_NAME), XSTR(ANGFSM_NAME)); \
    } \
    void ANGFSM_PASTE_EXPAND (angfsm_build_init_finalize_, ANGFSM_NAME)() \
    { \
        angfsm_build_init_finalize (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME)); \
    } \
    void ANGFSM_PASTE_EXPAND (angfsm_build_run_init_, ANGFSM_NAME)() \
    { \
        angfsm_build_run_init (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME)); \
    } \
    void ANGFSM_PASTE_EXPAND (angfsm_build_free_, ANGFSM_NAME)() \
    { \
        angfsm_build_free (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME)); \
    }

#define ANGFSM_IMPORT(fsm_name) \
extern angfsm_build_t angfsm_##fsm_name;

/** Pass options to angfsm though argc and argv. */
#define ANGFSM_OPTIONS(argc, argv) angfsm_build_options (argc, argv)

/** Generate source ou header files for the specific architecture */
#define ANGFSM_GENERATE(arch, string) angfsm_build_gen (#arch, string)

/** Immediatly handle an event by the fsm. */
#define ANGFSM_HANDLE(fsm, event) angfsm_build_handle_string (&angfsm_##fsm , #event)

/** Says if an event can actually be handle or not by the fsm. */
#define ANGFSM_CAN_HANDLE(fsm, event) \
    angfsm_build_can_handle_string (&angfsm_##fsm, #event)

/** Reset fsm to it's initial state. */
#define ANGFSM_RESET(fsm) angfsm_build_reset (&angfsm_##fsm)

/** Generate dot. */
#define ANGFSM_GEN_DOT(fsm, output) angfsm_build_gen_dot (&angfsm_##fsm, output)

/** Define states of the fsm, can be called several times to add other states. */
#define ANGFSM_STATES(states...) \
    void ANGFSM_PASTE3_EXPAND (fsm_build_states_, ANGFSM_NAME, FIRST (states)) () \
        __attribute__((constructor(103)));\
    void ANGFSM_PASTE3_EXPAND (fsm_build_states_, ANGFSM_NAME, FIRST (states)) () \
    { \
        angfsm_build_states (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME), #states); \
    }

/** Define events of the fsm, can be called several times to add other events. */
#define ANGFSM_EVENTS(events...) \
    void ANGFSM_PASTE3_EXPAND (fsm_build_events_, ANGFSM_NAME, FIRST (events)) () \
        __attribute__((constructor(103)));\
    void ANGFSM_PASTE3_EXPAND (fsm_build_events_, ANGFSM_NAME, FIRST (events)) () \
    { \
        angfsm_build_events (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME), #events); \
    }

/** Define wich state(s) will be active at the beginning. */
#define ANGFSM_START_WITH(starters...) \
    void ANGFSM_PASTE3_EXPAND (fsm_build_start_with_, ANGFSM_NAME, FIRST (starters))() __attribute__((constructor(104))); \
    void ANGFSM_PASTE3_EXPAND (fsm_build_start_with_, ANGFSM_NAME, FIRST (starters))() \
    { \
        angfsm_build_start_with (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME), #starters); \
    }

/**
 * Define a transition by giving a state, an event and:
 * - a event if the transition returns only one state.
 * - branches if the transition returns different states.
 * See examples for reel usage.
 */
#define ANGFSM_TRANS(state, event, output_branches...) \
    IFELSE_ARG1(void, uint, output_branches) ANGFSM_PASTE3_EXPAND (angfsm_trans_, ANGFSM_NAME,_##state##_##event) (); \
    void ANGFSM_PASTE3_EXPAND (angfsm_build_trans_, ANGFSM_NAME,_##state##_##event)() __attribute__((constructor(105))); \
    void ANGFSM_PASTE3_EXPAND (angfsm_build_trans_, ANGFSM_NAME,_##state##_##event)() \
    { \
        angfsm_build_trans (& ANGFSM_PASTE_EXPAND(angfsm_, ANGFSM_NAME), #state, #event, #output_branches, \
            (angfsm_build_run_strans_func_t) &ANGFSM_PASTE3_EXPAND (angfsm_trans_, ANGFSM_NAME,_##state##_##event)); \
    } \
    IFELSE_ARG1(void, uint, output_branches) ANGFSM_PASTE3_EXPAND (angfsm_trans_, ANGFSM_NAME,_##state##_##event) ()

/**
 * Define a transition when a state times out.
 * You have to provide a state, a timeout value and:
 * - a event if the transition returns only one state.
 * - branches if the transition returns different states.
 * See examples for reel usage.
 */
#define ANGFSM_TRANS_TIMEOUT(state, timeout, output_branches...) \
    void ANGFSM_PASTE3_EXPAND (angfsm_build_timeout_, ANGFSM_NAME,_##state##_TIMEOUT)() __attribute__((constructor(106))); \
    void ANGFSM_PASTE3_EXPAND (angfsm_build_timeout_, ANGFSM_NAME,_##state##_TIMEOUT)() \
    { \
        angfsm_build_timeout (& ANGFSM_PASTE_EXPAND(angfsm_, ANGFSM_NAME), #state, XSTR(state##_TIMEOUT), timeout); \
    } \
    ANGFSM_EVENTS (state##_TIMEOUT) \
    ANGFSM_TRANS (state, state##_TIMEOUT, output_branches)

/** Used to handle timeout events. */
#define ANGFSM_HANDLE_TIMEOUT(fsm_name) \
    (angfsm_build_handle_timeout (& angfsm_##fsm_name))

/** Transform an event in a numeric value. */
#define ANGFSM_EVENT(event) \
    (angfsm_build_get_event_code (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME), #event))
#define ANGFSM_EVENT_F(fsm_name, event) \
    (angfsm_build_get_event_code (& angfsm_##fsm_name, #event))

/** Transform a state in a numeric value. */
#define ANGFSM_STATE(state) \
    (angfsm_build_get_state (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME), #state)->code)
#define ANGFSM_STATE_F(fsm_name, state) \
    (angfsm_build_get_state (& angfsm_##fsm_name, #state)->code)

/** Transform a branch in a numeric value. */
#define ANGFSM_BRANCH(branch) \
    (angfsm_build_get_branch (& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME), #branch))
#define ANGFSM_BRANCH_F(fsm_name, branch) \
    (angfsm_build_get_branch (& angfsm_##fsm_name, #branch))

/** Handle event from numeric value. */
#define ANGFSM_HANDLE_VAR(fsm, event) \
    angfsm_build_handle_integer (& angfsm_##fsm, event)

/* Can we handle event from numeric value ? */
#define ANGFSM_CAN_HANDLE_VAR(fsm, event) \
    angfsm_build_can_handle_integer (& angfsm_##fsm, event)

/* Callback for transitions. */
#define ANGFSM_TRANS_CALLBACK(cb) \
    ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME).run.trans_callback = cb

/* Get state string. */
#define ANGFSM_STATE_STR(state) \
    ((char *)((angfsm_build_get_state_by_code((& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME)), state))->var_name))

/* Get event string. */
#define ANGFSM_EVENT_STR(event) \
    ((char *)((angfsm_build_get_event_by_code((& ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME)), event))->var_name))

/* Get branch string. */
#define ANGFSM_BRANCH_STR(branch) \
    ((char *)(ANGFSM_PASTE_EXPAND (angfsm_, ANGFSM_NAME).u_branch_name[branch]))

/**
 * Parse a string who contains a list of arguments separated by comma.
 * It will create and store each parameters (without space and ") in an array
 * and give the number of parsed arguments.
 * \param  string  string to parse
 * \param  tab  pointer to a table of strings where to store the array
 * \param  nb  pointer where to store the number of parsed arguments
 */
void
angfsm_build_arg_parse(char *string, char ***tab, int *nb);

/**
 * Free an array of strings generated by angfsm_build_arg_parse.
 * \param  tab  pointer to the array of strings
 * \param  nb  number of arguments stored in the array.
 */
void
angfsm_build_arg_free(char ***tab, int nb);

/**
 * Print transition.
 * \param  fsm  fsm
 * \param  trans transition where the return occurs
 * \param  branch  branch to transition has chosen.
 */
void
angfsm_build_print_trans (angfsm_build_t *fsm,
   angfsm_build_trans_t* trans,
   angfsm_build_branch_chain_t* branch);

/**
 * Test the fsm and search for errors.
 */
void
angfsm_build_sanity_check(angfsm_build_t *fsm);

/** Reset the fsm to it's initial state. */
void
angfsm_build_reset(angfsm_build_t *fsm);

/** Generate the dot file of the actual fsm state.
 * \param  fsm  fsm
 * \param  output  output file name. If empty or NULL, it will generate a file
 * like "angfsm_{FSM NAME}.dot" .
 */
void
angfsm_build_gen_dot(angfsm_build_t *fsm, char *output);

/** Initialize the fsm. */
void
angfsm_build_init(angfsm_build_t *fsm, char *name);

/** Finalize the preparation of the FSM informations. */
void
angfsm_build_init_finalize (angfsm_build_t *fsm);

/** Initialize the running data of the fsm. */
void
angfsm_build_run_init(angfsm_build_t *fsm);

/**
 * Add some states to the fsm.
 * \param  fsm  fsm
 * \param  states  states separated by comma
 **/
void
angfsm_build_states(angfsm_build_t *fsm, char *states);

/**
 * Add some events to the fsm.
 * \param  fsm  fsm
 * \param  events  events separated by comma
 **/
void
angfsm_build_events(angfsm_build_t *fsm, char *events);

/** Get event's pointer by giving it's name. */
angfsm_build_event_t*
angfsm_build_get_event(angfsm_build_t *fsm, char *event);

/** Get state's pointer by giving it's name. */
angfsm_build_state_t*
angfsm_build_get_state(angfsm_build_t *fsm, char *state);

/** Get event's pointer by giving it's code. */
angfsm_build_event_t*
angfsm_build_get_event_by_code(angfsm_build_t *fsm, uint event);

/** Get state's pointer by giving it's code. */
angfsm_build_state_t*
angfsm_build_get_state_by_code(angfsm_build_t *fsm, uint state);

/** Get transition pointer by giving it's state and event. */
angfsm_build_trans_t*
angfsm_build_get_trans (angfsm_build_t *fsm, uint state, uint event);

/** Get event code as a numeric value. */
uint16_t
angfsm_build_get_event_code(angfsm_build_t *fsm, char *event);

/** Get branch pointer from transition and branch string. */
angfsm_build_branch_chain_t*
angfsm_build_get_event_branch (angfsm_build_t *fsm,
                               angfsm_build_trans_t *trans,
                               uint branch);

/** Get branch unique code from branch name. */
uint
angfsm_build_get_branch (angfsm_build_t *fsm, char *branch);

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
angfsm_build_trans(angfsm_build_t *fsm,
        char *state,
        char *event,
        char *output_branches,
        angfsm_build_run_strans_func_t trans_func);

/* Create transition based on a timeout.
 * \param  fsm  fsm
 * \param  state  state who times out
 * \param  event  name of the timeout (specific to this state and described in
 macro)
 * \param  timeout  value of timeout
 */
void
angfsm_build_timeout(angfsm_build_t *fsm, char *state, char *event, uint timeout);

/**
 * Define with which states the fsm will start.
 * \param  fsm  fsm
 * \param  starters  list of states seperated by comma
 */
void
angfsm_build_start_with(angfsm_build_t *fsm, char *starters);

/** Handle an event by the fsm.
 * \param  fsm  fsm
 * \param  event event to handle with fsm
 * \return  zero if event is not handled at all, one otherwise
 **/
int
angfsm_build_handle(angfsm_build_t *fsm, angfsm_build_event_t *event);
int
angfsm_build_handle_string(angfsm_build_t *fsm, char *event);
int
angfsm_build_handle_integer(angfsm_build_t *fsm, uint16_t event);

/**
 * Say if the event can be handled or not.
 * \param  fsm  fsm
 * \param  event  event to test with the fsm
 * \return  zero if the event can be handled, non-zero otherwhise
 */
int
angfsm_build_can_handle(angfsm_build_t *fsm, angfsm_build_event_t *event);
int
angfsm_build_can_handle_string(angfsm_build_t *fsm, char *event);
int
angfsm_build_can_handle_integer(angfsm_build_t *fsm, uint16_t event);

/**
 * Handle timeout events of the fsm.
 * \param  fsm  fsm
 * \return  one if an event has been handled, zero otherwise
 */
int
angfsm_build_handle_timeout(angfsm_build_t *fsm);

/** Pass parameters to AngFSM at execution. Try --ang-help
 * \param  argc  argc from your main.
 * \param  argv  argv from your main.
 * \return  different from zero if any argument has been used, zero otherwhise.
 *  */
int
angfsm_build_options(int argc, char **argv);

/**
 * Generate header file for target which provides more optimised version of
 * fsm execution
 * \param  arch  specify generated architecture by it's string.
 * embedded).
 */
void
angfsm_build_gen(char *arch);
void
angfsm_build_gen_no_opti_h(angfsm_build_t *fsm, angfsm_build_arch_t arch);
void
angfsm_build_gen_no_opti_c(angfsm_build_t *fsm, angfsm_build_arch_t arch);
void
angfsm_build_gen_opti_avr_h(angfsm_build_t *fsm, angfsm_build_arch_t arch);
void
angfsm_build_gen_opti_avr_c(angfsm_build_t *fsm, angfsm_build_arch_t arch);

/** Free fsm allocated data. */
void
angfsm_build_free(angfsm_build_t *fsm);

/** Print help. */
void
angfsm_build_print_help();

#endif
