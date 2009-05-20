/* cylinder_cb.c - cylinder FSM callbacks. */
/*  {{{
 *
 * Copyright (C) 2009 Nicolas Haller
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
#include "cylinder_cb.h"
#include "asserv.h"
#include "cylinder.h"
#include "filterbridge.h"
#include "elevator.h"


/*
 * IDLE =start=>
 *  => WAIT_FOR_JACK_IN
 *   we wait the jack before moving anything
 */
fsm_branch_t
cylinder__IDLE__start (void)
{
    return cylinder_next (IDLE, start);
}

/*
 * WAIT_FOR_JACK_IN =jack_inserted_into_bot=>
 *  => RESET_POS
 *   we init the cylinder position
 */
fsm_branch_t
cylinder__WAIT_FOR_JACK_IN__jack_inserted_into_bot (void)
{
    asserv_arm_zero_position();
    return cylinder_next (WAIT_FOR_JACK_IN, jack_inserted_into_bot);
}

/*
 * RESET_POS =move_done=>
 *  => INIT_POS
 *   move the cylinder to open it
 */
fsm_branch_t
cylinder__RESET_POS__move_done (void)
{
    asserv_move_arm(CYLINDER_OFFSET,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (RESET_POS, move_done);
}

/*
 * INIT_POS =move_done=>
 *  => WAIT_A_PUCK
 *   the cylinder is ready to get pucks
 */
fsm_branch_t
cylinder__INIT_POS__move_done (void)
{
    return cylinder_next (INIT_POS, move_done);
}

/*
 * WAIT_A_PUCK =new_puck=>
 *  => TURN_PLUS_1
 *   turn cylinder 1 position after
 */
fsm_branch_t
cylinder__WAIT_A_PUCK__new_puck (void)
{
    asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_A_PUCK, new_puck);
}

/*
 * WAIT_A_PUCK =close_order=>
 *  => TURN_MINUS_1_CLOSE
 *   we close cylinder as requested
 */
fsm_branch_t
cylinder__WAIT_A_PUCK__close_order (void)
{
    asserv_move_arm(-1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_A_PUCK, close_order);
}

/*
 * WAIT_A_PUCK =flush_order=>
 *  => WAIT_BRIDGE_READY_FLUSH
 *   flush all pucks to the bridge
 */
fsm_branch_t
cylinder__WAIT_A_PUCK__flush_order (void)
{
    return cylinder_next (WAIT_A_PUCK, flush_order);
}

/*
 * TURN_PLUS_1 =move_done=>
 * of_no_puck => TURN_MINUS_1
 *   There is no puck, go backward
 * of_puck => WAIT_BRIDGE_READY
 *   There is a real puck, check if fb is ready to get puck
 */
fsm_branch_t
cylinder__TURN_PLUS_1__move_done (void)
{
    if(cylinder_puck_on_of)
      {
	++cylinder_nb_puck;
	return cylinder_next_branch (TURN_PLUS_1, move_done, of_puck);
      }
    return cylinder_next_branch (TURN_PLUS_1, move_done, of_no_puck);
}

/*
 * TURN_MINUS_1 =move_done=>
 *  => WAIT_A_PUCK
 *   wait a puck again
 */
fsm_branch_t
cylinder__TURN_MINUS_1__move_done (void)
{
    return cylinder_next (TURN_MINUS_1, move_done);
}

/*
 * WAIT_BRIDGE_READY =bridge_ready=>
 *  => TURN_PLUS_1_AGAIN
 *   open cylinder after the puck existence confirmation
 */
fsm_branch_t
cylinder__WAIT_BRIDGE_READY__bridge_ready (void)
{
    asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_BRIDGE_READY, bridge_ready);
}

/*
 * TURN_PLUS_1_AGAIN =move_done=>
 * bot_not_full => WAIT_A_PUCK
 *   ready for a new puck
 * bot_full => TURN_MINUS_1_CLOSE
 *   bot full we close the cylinder
 */
fsm_branch_t
cylinder__TURN_PLUS_1_AGAIN__move_done (void)
{
    if(cylinder_nb_puck == 2)
      {
	--cylinder_nb_puck;
	++fb_nb_puck;
      }
    if(fb_nb_puck+ cylinder_nb_puck + elvt_nb_puck < 4)
	return cylinder_next_branch (TURN_PLUS_1_AGAIN, move_done, bot_not_full);
    asserv_move_arm(-1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next_branch (TURN_PLUS_1_AGAIN, move_done, bot_full);
}

/*
 * TURN_MINUS_1_CLOSE =move_done=>
 *  => WAIT_BOT_NOT_FULL
 *   we wait the bot is not full to reopen cylinder
 */
fsm_branch_t
cylinder__TURN_MINUS_1_CLOSE__move_done (void)
{
    return cylinder_next (TURN_MINUS_1_CLOSE, move_done);
}

/*
 * WAIT_BOT_NOT_FULL =bot_not_full=>
 *  => WAIT_CLEAR_ORDER
 *   first condition to reopen bot is reached
 */
fsm_branch_t
cylinder__WAIT_BOT_NOT_FULL__bot_not_full (void)
{
    return cylinder_next (WAIT_BOT_NOT_FULL, bot_not_full);
}

/*
 * WAIT_BOT_NOT_FULL =flush_order=>
 *  => TURN_PLUS_1_FLUSH
 *   flush gordon
 */
fsm_branch_t
cylinder__WAIT_BOT_NOT_FULL__flush_order (void)
{
    asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_BOT_NOT_FULL, flush_order);
}

/*
 * WAIT_CLEAR_ORDER =no_close_order=>
 *  => TURN_PLUS_1_OPEN
 *   last condition to reopen bot reached, opening...
 */
fsm_branch_t
cylinder__WAIT_CLEAR_ORDER__no_close_order (void)
{
    asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_CLEAR_ORDER, no_close_order);
}

/*
 * WAIT_CLEAR_ORDER =flush_order=>
 *  => TURN_PLUS_1_FLUSH
 *   flush gordon
 */
fsm_branch_t
cylinder__WAIT_CLEAR_ORDER__flush_order (void)
{
    asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_CLEAR_ORDER, flush_order);
}

/*
 * TURN_PLUS_1_OPEN =move_done=>
 *  => WAIT_A_PUCK
 *   we wait a puck
 */
fsm_branch_t
cylinder__TURN_PLUS_1_OPEN__move_done (void)
{
    return cylinder_next (TURN_PLUS_1_OPEN, move_done);
}

/*
 * TURN_PLUS_1_FLUSH =move_done=>
 *  => WAIT_BRIDGE_READY_FLUSH
 *   cylinder open and ready to flush
 */
fsm_branch_t
cylinder__TURN_PLUS_1_FLUSH__move_done (void)
{
    return cylinder_next (TURN_PLUS_1_FLUSH, move_done);
}

/*
 * WAIT_BRIDGE_READY_FLUSH =bridge_ready=>
 *  => TURN_PLUS_2_FLUSH
 *   put a puck on the fb
 */
fsm_branch_t
cylinder__WAIT_BRIDGE_READY_FLUSH__bridge_ready (void)
{
    return cylinder_next (WAIT_BRIDGE_READY_FLUSH, bridge_ready);
}

/*
 * TURN_PLUS_2_FLUSH =move_done=>
 * cylinder_empty => TURN_MINUS_1_CLOSE
 *   flush finished
 * cylinder_not_empty => WAIT_BRIDGE_READY_FLUSH
 *   flush again
 */
fsm_branch_t
cylinder__TURN_PLUS_2_FLUSH__move_done (void)
{
    --cylinder_nb_puck;
    ++fb_nb_puck;
    if(!cylinder_nb_puck)
	return cylinder_next_branch (TURN_PLUS_2_FLUSH, move_done, cylinder_empty);
    return cylinder_next_branch (TURN_PLUS_2_FLUSH, move_done, cylinder_not_empty);
}


