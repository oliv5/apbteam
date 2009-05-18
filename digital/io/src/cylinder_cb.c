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
 *  => TURN_PLUS_3
 *   turn cylinder 3 position after
 */
fsm_branch_t
cylinder__WAIT_A_PUCK__new_puck (void)
{
    ++nb_puck_cylinder;
    asserv_move_arm(3*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_A_PUCK, new_puck);
}

/*
 * TURN_PLUS_3 =move_done=>
 *  => WAIT_FOR_BRIDGE_READY
 *   bridge full, waiting for Bison Fute's clearance
 */
fsm_branch_t
cylinder__TURN_PLUS_3__move_done (void)
{
    return cylinder_next (TURN_PLUS_3, move_done);
}

/*
 * WAIT_FOR_BRIDGE_READY =bridge_ready=>
 * no_puck_bo => TURN_PLUS_1
 *   bridge clear, and no other puck on cylinder
 * puck_bo => TURN_PLUS_2
 *   bridge clear, an another puck is in the cylinder
 */
fsm_branch_t
cylinder__WAIT_FOR_BRIDGE_READY__bridge_ready (void)
{
    ++nb_puck_fb;
    asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    if(!of_state)
      {
	--nb_puck_cylinder;
	return cylinder_next_branch (WAIT_FOR_BRIDGE_READY, bridge_ready, no_puck_bo);
      }
    return cylinder_next_branch (WAIT_FOR_BRIDGE_READY, bridge_ready, puck_bo);
}

/*
 * TURN_PLUS_1 =move_done=>
 * ok_for_other_puck => WAIT_A_PUCK
 *   cylinder empty and ready for new puck
 * not_ok_for_other_puck => TURN_MINUS_1
 *   bot full, closing cylinder
 */
fsm_branch_t
cylinder__TURN_PLUS_1__move_done (void)
{
    if(nb_puck_fb < 4)
	return cylinder_next_branch (TURN_PLUS_1, move_done, ok_for_other_puck);
    asserv_move_arm(-1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next_branch (TURN_PLUS_1, move_done, not_ok_for_other_puck);
}

/*
 * TURN_PLUS_2 =move_done=>
 * ok_for_other_puck => WAIT_FOR_BRIDGE_READY
 *   we test bo again
 * not_ok_for_other_puck => WAIT_FOR_PUCKS_RELEASE
 *   bot full, eject hypothetical puck and close the cylinder
 */
fsm_branch_t
cylinder__TURN_PLUS_2__move_done (void)
{
    if(nb_puck_fb < 4)
	return cylinder_next_branch (TURN_PLUS_2, move_done, ok_for_other_puck);
    return cylinder_next_branch (TURN_PLUS_2, move_done, not_ok_for_other_puck);
}

/*
 * TURN_MINUS_1 =move_done=>
 *  => WAIT_FOR_PUCKS_RELEASE
 *   cylinder close, you shall not pass (but try again later)
 */
fsm_branch_t
cylinder__TURN_MINUS_1__move_done (void)
{
    return cylinder_next (TURN_MINUS_1, move_done);
}

/*
 * WAIT_FOR_PUCKS_RELEASE =bot_empty=>
 *  => TURN_PLUS_1_AGAIN
 */
fsm_branch_t
cylinder__WAIT_FOR_PUCKS_RELEASE__bot_empty (void)
{
    asserv_move_arm(1*60*ASSERV_ARM_STEP_BY_DEGREE,
		    ASSERV_ARM_SPEED_DEFAULT);
    return cylinder_next (WAIT_FOR_PUCKS_RELEASE, bot_empty);
}

/*
 * TURN_PLUS_1_AGAIN =move_done=>
 *  => WAIT_A_PUCK
 *   cylinder ready
 */
fsm_branch_t
cylinder__TURN_PLUS_1_AGAIN__move_done (void)
{
    return cylinder_next (TURN_PLUS_1_AGAIN, move_done);
}


