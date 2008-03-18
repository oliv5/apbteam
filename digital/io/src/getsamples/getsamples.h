#ifndef getsamples_h
#define getsamples_h
/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * get_samples
 *
 * 
 */

typedef struct getsamples_t getsamples_t;

/* get_samples states. */
enum getsamples_state_t
{
    GETSAMPLES_STATE_PREPARE_ARM,
    GETSAMPLES_STATE_LOAD_SAMPLES,
    GETSAMPLES_STATE_FORWARD_CONTROL,
    GETSAMPLES_STATE_GO_TO_POSITION,
    GETSAMPLES_STATE_PREPARE_CLASSIFIER,
    GETSAMPLES_STATE_NB
};
typedef enum getsamples_state_t getsamples_state_t;

/* get_samples events. */
enum getsamples_event_t
{
    GETSAMPLES_EVENT_arm_prepared,
    GETSAMPLES_EVENT_move_finished,
    GETSAMPLES_EVENT_sample_loaded,
    GETSAMPLES_EVENT_move_blocked,
    GETSAMPLES_EVENT_classifer_prepared,
    GETSAMPLES_EVENT_ready_to_load,
    GETSAMPLES_EVENT_NB
};
typedef enum getsamples_event_t getsamples_event_t;

/* This macro enables checks for branches used in the wrong state/event
 * combination. */
#define _BRANCH(state, event, to) \
    ((GETSAMPLES_STATE_ ## state) << 16 \
     | (GETSAMPLES_EVENT_ ## event) << 8 \
     | (GETSAMPLES_STATE_ ## to))

/* get_samples branches. */
enum getsamples_branch_t
{
    GETSAMPLES_BRANCH__PREPARE_ARM__arm_prepared__ = _BRANCH (PREPARE_ARM, arm_prepared, PREPARE_CLASSIFIER),
    GETSAMPLES_BRANCH__FORWARD_CONTROL__move_finished__ = _BRANCH (FORWARD_CONTROL, move_finished, PREPARE_ARM),
    GETSAMPLES_BRANCH__GO_TO_POSITION__move_blocked__ = _BRANCH (GO_TO_POSITION, move_blocked, GO_TO_POSITION),
    GETSAMPLES_BRANCH__GO_TO_POSITION__move_finished__ = _BRANCH (GO_TO_POSITION, move_finished, FORWARD_CONTROL),
    GETSAMPLES_BRANCH__PREPARE_CLASSIFIER__classifer_prepared__ = _BRANCH (PREPARE_CLASSIFIER, classifer_prepared, LOAD_SAMPLES),
};
typedef enum getsamples_branch_t getsamples_branch_t;

#undef _BRANCH

/* get_samples transition type. */
typedef getsamples_branch_t (*getsamples_transition_t) (getsamples_t *user);

/* Initialise get_samples automaton. */
void
getsamples_init (getsamples_t *user);

/* Handle events on get_samples automaton. */
void
getsamples_handle_event (getsamples_t *user, getsamples_event_t event);

/* Value to return to follow the only branch. */
#define getsamples_next(state, event) \
    GETSAMPLES_BRANCH__ ## state ## __ ## event ## __

/* Value to return to follow a given branch. */
#define getsamples_next_branch(state, event, branch) \
    GETSAMPLES_BRANCH__ ## state ## __ ## event ## __ ## branch

#endif /* getsamples_h */
