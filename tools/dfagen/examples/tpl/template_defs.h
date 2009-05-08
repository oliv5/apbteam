#ifndef %(prefix)s_defs_h
#define %(prefix)s_defs_h
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
typedef enum %(prefix)s_state_t %(prefix)s_state_t;

/* %(name)s events. */
enum %(prefix)s_event_type_t
{
%(events,%(PREFIX)s_EVENT_TYPE_%(event)s)s    %(PREFIX)s_EVENT_TYPE_NB
};
typedef enum %(prefix)s_event_type_t %(prefix)s_event_type_t;

/* Only care about next state. */
#define _BRANCH(state, event, to) (%(PREFIX)s_STATE_ ## to)

/* %(name)s branches. */
enum %(prefix)s_branch_t
{
%(branches)s};
typedef enum %(prefix)s_branch_t %(prefix)s_branch_t;

#undef _BRANCH

/* %(name)s transition type. */
typedef %(prefix)s_branch_t (*%(prefix)s_transition_t) (void);

/* Value to return to follow the only branch. */
#define %(prefix)s_next(state, event) \
    %(PREFIX)s_BRANCH__ ## state ## __ ## event ## __

/* Value to return to follow a given branch. */
#define %(prefix)s_next_branch(state, event, branch) \
    %(PREFIX)s_BRANCH__ ## state ## __ ## event ## __ ## branch

/* Number of initial events. */
#define %(PREFIX)s_INITIALS_NB %(initials_nb)s

#endif /* %(prefix)s_defs_h */
