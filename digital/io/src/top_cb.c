/* top_cb.c - top FSM callbacks. */
/*  {{{
 *
 * Copyright (C) 2008 Jérémy Dufour
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */
#include "common.h"
#include "fsm.h"
#include "top_cb.h"

#include "move.h"	/* move FSM */
#include "playground.h"	/* PG_* */
#include "asserv.h"	/* asserv_* */
#include "chrono.h"	/* chrono_init */
/* AVR include, non HOST */
#ifndef HOST
# include "switch.h"	/* switch_get_color */
#endif /* HOST */
#include "simu.host.h"

#include "getsamples.h"	/* getsamples_* */
#include "gutter.h"	/* gutter_start */

/**
 * When we need to tell the main loop we want to be alerted when the last
 * command sent to the asserv board has been acknowledged.
 */
extern uint8_t top_waiting_for_settings_ack_;

/*
 * DROP_OFF_BALLS_TO_GUTTER =gutter_fsm_finished=>
 *  => GO_TO_SAMPLE_DISTRIBUTOR
 *   we have finished to drop off all the balls, let's go to our sample
 *   ditributor to try the same strategy again
 *   reset internal data
 */
fsm_branch_t
top__DROP_OFF_BALLS_TO_GUTTER__gutter_fsm_finished (void)
{
    /* Start the move FSM */
    move_start (PG_DISTRIBUTOR_SAMPLE_OUR_X, PG_DISTRIBUTOR_SAMPLE_OUR_Y);
    return top_next (DROP_OFF_BALLS_TO_GUTTER, gutter_fsm_finished);
}

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
    bot_color = switch_get_color ();
    /* Start the chronometer */
    chrono_init ();
    /* Reset the position of the bot */
    asserv_set_position (PG_X_START, PG_Y_START, PG_A_START);
    /* Tell the main loop we need to be aware when the asserv acknowledge our
     * settings command */
    top_waiting_for_settings_ack_ = 1;
    return top_next (WAIT_JACK_OUT, jack_removed_from_bot);
}

/*
 * GET_SAMPLES_FROM_SAMPLES_DISTRIBUTOR =get_samples_fsm_finished=>
 *  => GO_TO_OUR_ICE_DISTRIBUTOR
 *   we have finished to get our samples, let's go to our ice distributor with
 *   the move FSM
 */
fsm_branch_t
top__GET_SAMPLES_FROM_SAMPLES_DISTRIBUTOR__get_samples_fsm_finished (void)
{
    /* Start the move FSM to our ice distributor */
    move_start (PG_DISTRIBUTOR_ICE_OUR_X, PG_DISTRIBUTOR_ICE_OUR_Y);
    return top_next (GET_SAMPLES_FROM_SAMPLES_DISTRIBUTOR, get_samples_fsm_finished);
}

/*
 * GO_TO_ADVERSE_ICE_DISTRIBUTOR =move_fsm_finished=>
 *  => GET_ICE_FROM_ADVERSE_ICE_DISTRIBUTOR
 *   we are now in front of the adverse ice distributor, launch the get samples
 *   FSM
 */
fsm_branch_t
top__GO_TO_ADVERSE_ICE_DISTRIBUTOR__move_fsm_finished (void)
{
    /* Start the get samples FSM with the correct angle */
    /* TODO: where to put the ice?! */
    getsamples_start (PG_DISTRIBUTOR_ICE_ADVERSE_A, 0);
    return top_next (GO_TO_ADVERSE_ICE_DISTRIBUTOR, move_fsm_finished);
}

/*
 * GO_TO_OUR_ICE_DISTRIBUTOR =move_fsm_finished=>
 *  => GET_ICE_FROM_OUR_ICE_DISTRIBUTOR
 *   we are now in front of our ice distributor, launch the get samples FSM
 */
fsm_branch_t
top__GO_TO_OUR_ICE_DISTRIBUTOR__move_fsm_finished (void)
{
    /* Start the get samples FSM with the correct angle */
    /* TODO: where to put the ice?! */
    getsamples_start (PG_DISTRIBUTOR_ICE_OUR_A, 0);
    return top_next (GO_TO_OUR_ICE_DISTRIBUTOR, move_fsm_finished);
}

/*
 * GO_TO_SAMPLE_DISTRIBUTOR =move_fsm_finished=>
 *  => GET_SAMPLES_FROM_SAMPLES_DISTRIBUTOR
 *   we are now in front of our samples distributor, launch the get samples FSM
 */
fsm_branch_t
top__GO_TO_SAMPLE_DISTRIBUTOR__move_fsm_finished (void)
{
    /* Start the get samples FSM with the correct angle */
    /* TODO: where to put the samples?! */
    getsamples_start (PG_DISTRIBUTOR_SAMPLE_OUR_A, 0);
    return top_next (GO_TO_SAMPLE_DISTRIBUTOR, move_fsm_finished);
}

/*
 * IDLE =start=>
 *  => WAIT_JACK_IN
 *   tell the main loop we want to be informed when the jack is inserted into the
 *   bot
 */
fsm_branch_t
top__IDLE__start (void)
{
    /* TODO */
    return top_next (IDLE, start);
}

/*
 * GO_TO_GUTTER =move_fsm_finished=>
 *  => DROP_OFF_BALLS_TO_GUTTER
 *   we are now at the gutter, let's drop all ours balls into it with the gutter
 *   FSM
 */
fsm_branch_t
top__GO_TO_GUTTER__move_fsm_finished (void)
{
    /* Start the gutter FSM */
    gutter_start ();
    return top_next (GO_TO_GUTTER, move_fsm_finished);
}

/*
 * GET_ICE_FROM_OUR_ICE_DISTRIBUTOR =get_samples_fsm_finished=>
 * not full => GO_TO_ADVERSE_ICE_DISTRIBUTOR
 *   we have finished to get ice from our distributor and we have some space
 *   left, let's go the adverse ice distributor with the move FSM
 * full => GO_TO_GUTTER
 *   we have finished to get ice from our distributor and we have no more space
 *   left, let's go the gutter with the move FSM
 */
fsm_branch_t
top__GET_ICE_FROM_OUR_ICE_DISTRIBUTOR__get_samples_fsm_finished (void)
{
    /* TODO: how to detect we have no more space? */
    if (0)
      {
	/* Start the move FSM to the adverse ice distributor */
	move_start (PG_DISTRIBUTOR_ICE_ADVERSE_X, PG_DISTRIBUTOR_ICE_ADVERSE_Y);
	return top_next_branch (GET_ICE_FROM_OUR_ICE_DISTRIBUTOR, get_samples_fsm_finished, not_full);
      }
    else
      {
	/* Start the move FSM to go to the gutter */
	move_start (PG_GUTTER_X, PG_GUTTER_Y);
	return top_next_branch (GET_ICE_FROM_OUR_ICE_DISTRIBUTOR, get_samples_fsm_finished, full);
      }
}

/*
 * GET_ICE_FROM_ADVERSE_ICE_DISTRIBUTOR =get_samples_fsm_finished=>
 *  => GO_TO_GUTTER
 *   we have finished to get ice. Even if we are not full, let's go to the gutter
 *   with the move FSM
 */
fsm_branch_t
top__GET_ICE_FROM_ADVERSE_ICE_DISTRIBUTOR__get_samples_fsm_finished (void)
{
    /* Start the move FSM to go to the gutter */
    move_start (PG_GUTTER_X, PG_GUTTER_Y);
    return top_next (GET_ICE_FROM_ADVERSE_ICE_DISTRIBUTOR, get_samples_fsm_finished);
}

/*
 * WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => WAIT_JACK_OUT
 *   tell the main loop we want to be informed when the jack is removed from the
 *   bot
 */
fsm_branch_t
top__WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    /* TODO */
    return top_next (WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * CONFIGURE_ASSERV =settings_acknowledged=>
 *  => GO_TO_SAMPLE_DISTRIBUTOR
 *   order the bot to move to our samples distributors with the move FSM
 */
fsm_branch_t
top__CONFIGURE_ASSERV__settings_acknowledged (void)
{
    /* Clear the flag for the setting acknowleged */
    top_waiting_for_settings_ack_ = 0;
    /* Start the move FSM to our samples distributor */
    move_start (PG_DISTRIBUTOR_SAMPLE_OUR_X, PG_DISTRIBUTOR_SAMPLE_OUR_Y);
    return top_next (CONFIGURE_ASSERV, settings_acknowledged);
}


