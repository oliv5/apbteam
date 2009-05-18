/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * Skeleton for top callbacks implementation.
 *
 * Top FSM with the Marcel's strategy
 */
#include "common.h"
#include "fsm.h"
#include "top_cb.h"
#include "chrono.h"
#include "asserv.h"
#include "playground.h"
#include "move.h"	/* move FSM */

/*
 * IDLE =start=>
 *  => WAIT_FIRST_JACK_IN
 *   nothing to do.
 */
fsm_branch_t
top__IDLE__start (void)
{
    return top_next (IDLE, start);
}

/*
 * WAIT_FIRST_JACK_IN =jack_inserted_into_bot=>
 *  => WAIT_FIRST_JACK_OUT
 *   nothing to do.
 */
fsm_branch_t
top__WAIT_FIRST_JACK_IN__jack_inserted_into_bot (void)
{
    return top_next (WAIT_FIRST_JACK_IN, jack_inserted_into_bot);
}

/*
 * WAIT_FIRST_JACK_OUT =jack_removed_from_bot=>
 *  => WAIT_SECOND_JACK_IN
 *   nothing to do.
 */
fsm_branch_t
top__WAIT_FIRST_JACK_OUT__jack_removed_from_bot (void)
{
    return top_next (WAIT_FIRST_JACK_OUT, jack_removed_from_bot);
}

/*
 * WAIT_SECOND_JACK_IN =jack_removed_from_bot=>
 *  => WAIT_SECOND_JACK_OUT
 *   nothing to do.
 */
fsm_branch_t
top__WAIT_SECOND_JACK_IN__jack_removed_from_bot (void)
{
    return top_next (WAIT_SECOND_JACK_IN, jack_removed_from_bot);
}

/*
 * WAIT_SECOND_JACK_OUT =jack_removed_from_bot=>
 *  => GET_PUCK_FROM_THE_GROUND
 *   the match start, try to get some puck from the ground.
 */
fsm_branch_t
top__WAIT_SECOND_JACK_OUT__jack_removed_from_bot (void)
{
    /* Start the chronometer */
    chrono_init ();
    return top_next (WAIT_SECOND_JACK_OUT, jack_removed_from_bot);
}


