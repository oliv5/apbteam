/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * get_samples
 *
 * 
 */
#include "getsamples_cb.h"
#include <stddef.h>
#include <assert.h>

struct getsamples_t { getsamples_state_t fsm; };
 
/* get_samples transition table. */
static const getsamples_transition_t
getsamples_transition_table[GETSAMPLES_STATE_NB][GETSAMPLES_EVENT_NB] = {
    { getsamples__PREPARE_ARM__arm_prepared,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL },
    { NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL },
    { NULL,
      getsamples__FORWARD_CONTROL__move_finished,
      NULL,
      NULL,
      NULL,
      NULL },
    { NULL,
      getsamples__GO_TO_POSITION__move_finished,
      NULL,
      getsamples__GO_TO_POSITION__move_blocked,
      NULL,
      NULL },
    { NULL,
      NULL,
      NULL,
      NULL,
      getsamples__PREPARE_CLASSIFIER__classifer_prepared,
      NULL },
};

/* Initialise get_samples automaton. */
void
getsamples_init (getsamples_t *user)
{
    user->fsm = GETSAMPLES_STATE_GO_TO_POSITION;
}

/* Handle events on get_samples automaton. */
void
getsamples_handle_event (getsamples_t *user, getsamples_event_t event)
{
    assert (user);
    getsamples_state_t state = user->fsm;
    assert (state < GETSAMPLES_STATE_NB);
    assert (event < GETSAMPLES_EVENT_NB);
    getsamples_transition_t tr = getsamples_transition_table[state][event];
    assert (tr);
    getsamples_branch_t br = tr (user);
    assert (((br >> 16) & 0xff) == state);
    assert (((br >> 8) & 0xff) == event);
    user->fsm = br & 0xff;
}

