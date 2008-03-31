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

/* %(name)s context. */
fsm_t %(prefix)s_fsm = {
    &%(prefix)s_transition_table[0][0],
    %(PREFIX)s_EVENT_NB,
    %(PREFIX)s_STATE_%(initial)s,
    %(PREFIX)s_STATE_%(initial)s,
};

