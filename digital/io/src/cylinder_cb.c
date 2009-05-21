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
#include "top.h"

/* locales variables */
/* is there a puck on pos 2 or 3 */
uint8_t puck_on_cylinder = 0;
/* is the cylinder are in of_offset mode */
uint8_t of_offset_enabled = 0;

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
 *  => WAIT_BRIDGE_READY
 *   look if the bridge is ready before move
 */
fsm_branch_t
cylinder__WAIT_A_PUCK__new_puck (void)
{
    return cylinder_next (WAIT_A_PUCK, new_puck);
}

/*
 * WAIT_A_PUCK =close_order=>
 *  => TURN_PLUS_1_CLOSE
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
 * WAIT_BRIDGE_READY =bridge_ready=>
 *  => TURN_PLUS_1_AND_OFO
 *   open the cylinder with the puck or not.
 */
fsm_branch_t
cylinder__WAIT_BRIDGE_READY__bridge_ready (void)
{
    of_offset_enabled = 1;
    asserv_move_arm((1+CYLINDER_OF_OFFSET)*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_BRIDGE_READY, bridge_ready);
}

/*
 * TURN_PLUS_1_AND_OFO =move_done=>
 * bot_not_full => TURN_PLUS_1_AND_MINUS_OFO
 *   open the cylinder to wait a new puck
 * bot_full => WAIT_BOT_NOT_FULL
 *   bot full, waiting for pucks teleportation
 */
fsm_branch_t
cylinder__TURN_PLUS_1_AND_OFO__move_done (void)
{
    /* we verify if we drop a puck to the bridge */
    if(puck_on_cylinder)
      {
	--cylinder_nb_puck;
	++fb_nb_puck;
      }
    /* We probe the OF to see if we have a new puck */
    puck_on_cylinder = asserv_arm_of_status();
    if(puck_on_cylinder)
      {
	++top_total_puck_taken;
	++top_puck_inside_bot;
	++cylinder_nb_puck;
      }
    if(top_puck_inside_bot < 4)
      {
	of_offset_enabled = 0;
	asserv_move_arm((1-CYLINDER_OF_OFFSET)*60*ASSERV_ARM_STEP_BY_DEGREE,
			ASSERV_ARM_SPEED_DEFAULT);
	return cylinder_next_branch (TURN_PLUS_1_AND_OFO, move_done, bot_not_full);
      }
    return cylinder_next_branch (TURN_PLUS_1_AND_OFO, move_done, bot_full);
}

/*
 * TURN_PLUS_1_AND_MINUS_OFO =move_done=>
 *  => WAIT_A_PUCK
 *   ready for other pucks
 */
fsm_branch_t
cylinder__TURN_PLUS_1_AND_MINUS_OFO__move_done (void)
{
    return cylinder_next (TURN_PLUS_1_AND_MINUS_OFO, move_done);
}

/*
 * WAIT_BOT_NOT_FULL =bot_not_full=>
 *  => WAIT_CLEAR_ORDER
 *   the bot is not full, we go testing the other close condition
 */
fsm_branch_t
cylinder__WAIT_BOT_NOT_FULL__bot_not_full (void)
{
    return cylinder_next (WAIT_BOT_NOT_FULL, bot_not_full);
}

/*
 * WAIT_BOT_NOT_FULL =flush_order=>
 *  => TURN_PLUS_1_FLUSH
 *   flush order received, go open the cylinder
 */
fsm_branch_t
cylinder__WAIT_BOT_NOT_FULL__flush_order (void)
{
    if(of_offset_enabled)
      {
	asserv_move_arm((1-CYLINDER_OF_OFFSET)*60*ASSERV_ARM_STEP_BY_DEGREE,
			ASSERV_ARM_SPEED_DEFAULT);
	of_offset_enabled = 0;
      }
    else
	asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
			ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_BOT_NOT_FULL, flush_order);
}

/*
 * WAIT_CLEAR_ORDER =no_close_order=>
 *  => TURN_PLUS_1_OPEN
 *   no close order, we reopen cylinder to get other pucks
 */
fsm_branch_t
cylinder__WAIT_CLEAR_ORDER__no_close_order (void)
{
    if(of_offset_enabled)
      {
	asserv_move_arm((1-CYLINDER_OF_OFFSET)*60*ASSERV_ARM_STEP_BY_DEGREE,
			ASSERV_ARM_SPEED_DEFAULT);
	of_offset_enabled = 0;
      }
    else
	asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
			ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_CLEAR_ORDER, no_close_order);
}

/*
 * WAIT_CLEAR_ORDER =flush_order=>
 *  => TURN_PLUS_1_FLUSH
 *   flush order received, go open the cylinder
 */
fsm_branch_t
cylinder__WAIT_CLEAR_ORDER__flush_order (void)
{
    if(of_offset_enabled)
      {
	asserv_move_arm((1-CYLINDER_OF_OFFSET)*60*ASSERV_ARM_STEP_BY_DEGREE,
			ASSERV_ARM_SPEED_DEFAULT);
	of_offset_enabled = 0;
      }
    else
	asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
			ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_CLEAR_ORDER, flush_order);
}

/*
 * TURN_PLUS_1_OPEN =move_done=>
 *  => WAIT_A_PUCK
 *   cylinder ready to get other pucks
 */
fsm_branch_t
cylinder__TURN_PLUS_1_OPEN__move_done (void)
{
    return cylinder_next (TURN_PLUS_1_OPEN, move_done);
}

/*
 * TURN_PLUS_1_FLUSH =move_done=>
 *  => WAIT_BRIDGE_READY_FLUSH
 *   we wait the bridge before moving
 */
fsm_branch_t
cylinder__TURN_PLUS_1_FLUSH__move_done (void)
{
    return cylinder_next (TURN_PLUS_1_FLUSH, move_done);
}

/*
 * WAIT_BRIDGE_READY_FLUSH =bridge_ready=>
 *  => TURN_PLUS_3_FLUSH
 *   bridge is ready, flush gordon
 */
fsm_branch_t
cylinder__WAIT_BRIDGE_READY_FLUSH__bridge_ready (void)
{
    asserv_move_arm(3*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_BRIDGE_READY_FLUSH, bridge_ready);
}

/*
 * TURN_PLUS_3_FLUSH =move_done=>
 *  => WAIT_BOT_NOT_FULL
 *   cylinder flushed, we test the 2 close conditions before reopen cylinder
 */
fsm_branch_t
cylinder__TURN_PLUS_3_FLUSH__move_done (void)
{
    cylinder_flush_order = 0;
    if(puck_on_cylinder)
      {
	--cylinder_nb_puck;
	++fb_nb_puck;
	puck_on_cylinder = 0;
      }
    return cylinder_next (TURN_PLUS_3_FLUSH, move_done);
}

/*
 * TURN_PLUS_1_CLOSE =move_done=>
 *  => WAIT_BOT_NOT_FULL
 *   close order executed, test the 2 close conditions before reopen cylinder
 */
fsm_branch_t
cylinder__TURN_PLUS_1_CLOSE__move_done (void)
{
    if(puck_on_cylinder)
      {
	--cylinder_nb_puck;
	++fb_nb_puck;
	puck_on_cylinder = 0;
      }
    return cylinder_next (TURN_PLUS_1_CLOSE, move_done);
}
