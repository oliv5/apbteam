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
 * WAIT_JACK_OUT =jack_removed_from_bot=>
 *  => CONFIGURE_ASSERV
 *   the match start, start the chronometer
 *   we should also initialize all the subsystems of IO (get our color, ...)
 *   set the settings of the asserv board (especially the position)
 */
fsm_branch_t
top__WAIT_JACK_OUT__jack_removed_from_bot (void)
{
    /* Set-up our color */
    //bot_color = switch_get_color ();
    /* Start the chronometer */
    chrono_init ();
    /* Reset the position of the bot */
    asserv_set_position (PG_X_START, PG_Y_START, PG_A_START);
    /* Tell the main loop we need to be aware when the asserv acknowledge our
     * settings command */
    //top_waiting_for_settings_ack_ = 1;
    return top_next (WAIT_JACK_OUT, jack_removed_from_bot);
}

/*
 * MOVE_BACKWARD =move_fsm_finished=>
 *  => MOVE_FORWARD
 *   move forward again
 */
fsm_branch_t
top__MOVE_BACKWARD__move_fsm_finished (void)
{
    /* Get current position */
    asserv_position_t position;
    asserv_get_position (&position);
    /* FIXME: linear move is better? */
    move_start (position.x, position.y - 1500, 0);
    return top_next (MOVE_BACKWARD, move_fsm_finished);
}

/*
 * IDLE =start=>
 *  => WAIT_JACK_IN
 *   wait for the jack to be inserted into the bot
 */
fsm_branch_t
top__IDLE__start (void)
{
    //top_fsm_loop_count_ = 0;
    return top_next (IDLE, start);
}

/*
 * MOVE_FORWARD =move_fsm_finished=>
 *  => MOVE_BACKWARD
 *   return to initial position (yep it's useless but it's for test)
 */
fsm_branch_t
top__MOVE_FORWARD__move_fsm_finished (void)
{
    /* Get current position */
    asserv_position_t position;
    asserv_get_position (&position);
    /* FIXME: linear move is better? */
    move_start (position.x, position.y + 1500, 0);
    return top_next (MOVE_FORWARD, move_fsm_finished);
}

/*
 * WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => WAIT_JACK_OUT
 *   wait for the jack to be removed from the bot
 */
fsm_branch_t
top__WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    return top_next (WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * CONFIGURE_ASSERV =settings_acknowledged=>
 *  => MOVE_FORWARD
 *   move the bot forward
 */
fsm_branch_t
top__CONFIGURE_ASSERV__settings_acknowledged (void)
{
    /* Get current position */
    asserv_position_t position;
    asserv_get_position (&position);
    /* FIXME: linear move is better? */
    move_start (position.x, position.y - 1500, 0);
    return top_next (CONFIGURE_ASSERV, settings_acknowledged);
}


