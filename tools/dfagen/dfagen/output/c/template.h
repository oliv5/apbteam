#ifndef %(prefix)s_h
#define %(prefix)s_h
/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * %(name)s
 *
%(*comments)s */

%(_user.type-forward-decl)s
/* %(name)s states. */
enum %(prefix)s_state_t
{
%(states)s    %(PREFIX)s_STATE_NB
};
typedef enum %(prefix)s_state_t %(prefix)s_state_t;

/* %(name)s events. */
enum %(prefix)s_event_t
{
%(events)s    %(PREFIX)s_EVENT_NB
};
typedef enum %(prefix)s_event_t %(prefix)s_event_t;

/* This macro enables checks for branches used in the wrong state/event
 * combination. */
#define _BRANCH(state, event, to) \
    ((%(PREFIX)s_STATE_ ## state) << 16 \
     | (%(PREFIX)s_EVENT_ ## event) << 8 \
     | (%(PREFIX)s_STATE_ ## to))

/* %(name)s branches. */
enum %(prefix)s_branch_t
{
%(branches)s};
typedef enum %(prefix)s_branch_t %(prefix)s_branch_t;

#undef _BRANCH

/* %(name)s transition type. */
typedef %(prefix)s_branch_t (*%(prefix)s_transition_t) (%(user.type)s *user);

/* Initialise %(name)s automaton. */
void
%(prefix)s_init (%(user.type)s *user);

/* Handle events on %(name)s automaton. */
void
%(prefix)s_handle_event (%(user.type)s *user, %(prefix)s_event_t event);

/* Value to return to follow the only branch. */
#define %(prefix)s_next(state, event) \
    %(PREFIX)s_BRANCH__ ## state ## __ ## event ## __

/* Value to return to follow a given branch. */
#define %(prefix)s_next_branch(state, event, branch) \
    %(PREFIX)s_BRANCH__ ## state ## __ ## event ## __ ## branch

#endif /* %(prefix)s_h */
