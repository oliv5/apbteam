/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * %(name)s
 *
%(*comments)s */

/* %(name)s transition table. */
static const %(prefix)s_transition_t
%(prefix)s_transition_table[%(PREFIX)s_STATE_NB][%(PREFIX)s_EVENT_TYPE_NB] = {
%(transition_table)s};

/* %(name)s only branch table. */
static const %(prefix)s_state_t
%(prefix)s_only_table[%(PREFIX)s_STATE_NB][%(PREFIX)s_EVENT_TYPE_NB] = {
%(only_branch_table)s};

/* %(name)s initial states. */
static const %(prefix)s_state_t
%(prefix)s_initials_table[] = {
%(initials)s};

/* %(name)s state attributes. */
static const %(prefix)s_state_t
%(prefix)s_attr_table[] = {
%(states,"%(state)s (%(@)s) (%(@in)s) (%(@out|no_out)s)")s};
