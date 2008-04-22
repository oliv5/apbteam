/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * %(name)s
 *
%(*comments)s */
#include "common.h"
#include "fsm.h"
#include "%(prefix)s_cb.h"

#define NULL ((void *)0L)

/* %(name)s transition table. */
static const fsm_transition_t
%(prefix)s_transition_table[%(PREFIX)s_STATE_NB][%(PREFIX)s_EVENT_NB] = {
%(transition_table)s};

#ifdef HOST

/* %(name)s state names. */
static const char *
%(prefix)s_states_names[] = {
%(states_names)s};

/* %(name)s event names. */
static const char *
%(prefix)s_events_names[] = {
%(events_names)s};

#endif /* HOST */

/* %(name)s context. */
fsm_t %(prefix)s_fsm = {
    &%(prefix)s_transition_table[0][0],
    %(PREFIX)s_EVENT_NB,
    %(PREFIX)s_STATE_%(initial)s,
    %(PREFIX)s_STATE_%(initial)s,
#ifdef HOST
    "%(name)s",
    %(prefix)s_states_names,
    %(prefix)s_events_names,
#endif
};

