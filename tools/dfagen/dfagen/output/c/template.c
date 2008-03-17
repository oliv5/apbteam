/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * %(name)s
 *
%(*comments)s */
#include "%(prefix)s_cb.h"
#include <stddef.h>
#include <assert.h>

%(_user.type-decl)s 
/* %(name)s transition table. */
static const %(prefix)s_transition_t
%(prefix)s_transition_table[%(PREFIX)s_STATE_NB][%(PREFIX)s_EVENT_NB] = {
%(transition_table)s};

/* Initialise %(name)s automaton. */
void
%(prefix)s_init (%(user.type)s *user)
{
    user->%(user.field)s = %(PREFIX)s_STATE_%(initial)s;
}

/* Handle events on %(name)s automaton. */
void
%(prefix)s_handle_event (%(user.type)s *user, %(prefix)s_event_t event)
{
    assert (user);
    %(prefix)s_state_t state = user->%(user.field)s;
    assert (state < %(PREFIX)s_STATE_NB);
    assert (event < %(PREFIX)s_EVENT_NB);
    %(prefix)s_transition_t tr = %(prefix)s_transition_table[state][event];
    assert (tr);
    %(prefix)s_branch_t br = tr (user);
    assert (((br >> 16) & 0xff) == state);
    assert (((br >> 8) & 0xff) == event);
    user->%(user.field)s = br & 0xff;
}

