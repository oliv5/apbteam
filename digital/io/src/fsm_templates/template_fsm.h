#ifndef %(prefix)s_fsm_h
#define %(prefix)s_fsm_h
/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * %(name)s
 *
%(*comments)s */

/* %(name)s states. */
enum %(prefix)s_state_t
{
%(states)s    %(PREFIX)s_STATE_NB
};

/* %(name)s events. */
enum %(prefix)s_event_t
{
%(events)s    %(PREFIX)s_EVENT_NB
};

/* This macro enables checks for branches used in the wrong state/event
 * combination. */
#ifdef HOST
# define _BRANCH(state, event, to) \
    ((%(PREFIX)s_STATE_ ## state) << 16 \
     | (%(PREFIX)s_EVENT_ ## event) << 8 \
     | (%(PREFIX)s_STATE_ ## to))
#else
# define _BRANCH(state, event, to) \
     ((%(PREFIX)s_STATE_ ## to))
#endif

/* %(name)s branches. */
enum %(prefix)s_branch_t
{
%(branches)s};

#undef _BRANCH

/* Value to return to follow the only branch. */
#define %(prefix)s_next(state, event) \
    %(PREFIX)s_BRANCH__ ## state ## __ ## event ## __

/* Value to return to follow a given branch. */
#define %(prefix)s_next_branch(state, event, branch) \
    %(PREFIX)s_BRANCH__ ## state ## __ ## event ## __ ## branch

/* %(name)s context. */
extern fsm_t %(prefix)s_fsm;

#endif /* %(prefix)s_fsm_h */
