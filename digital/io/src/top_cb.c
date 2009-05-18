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
 *  => WAIT_JACK_IN
 *   nothing to do.
 */
fsm_branch_t
top__IDLE__start (void)
{
    return top_next (IDLE, start);
}

/*
 * WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => WAIT_JACK_OUT
 *   configure the asserv board.
 */
fsm_branch_t
top__WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    return top_next (WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * WAIT_JACK_OUT =jack_removed_from_bot=>
 *  => GET_PUCK_FROM_THE_GROUND
 *   the match start, start the chronometer
 */
fsm_branch_t
top__WAIT_JACK_OUT__jack_removed_from_bot (void)
{
    /* Start the chronometer */
    chrono_init ();
    return top_next (WAIT_JACK_OUT, jack_removed_from_bot);
}


