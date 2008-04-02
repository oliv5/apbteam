/* getsamples_cb.c - getsamples FSM callbacks. */
/*  {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include "getsamples_cb.h"
#include "getsamples.h"
#include "asserv.h"

/*
 * PREPARE_ARM =arm_moved=>
 *  => FORWARD_CONTROL
 *   Prepare the arm to the correct position.
 */
fsm_branch_t
getsamples__PREPARE_ARM__arm_moved (void)
{
    // final 
    asserv_go_to_distributor (); 
    return getsamples_next (PREPARE_ARM, arm_moved);
}

/*
 * FORWARD_CONTROL =position_reached=>
 *  => TAKE_SAMPLES
 *   End the position to the distributor.
 */
fsm_branch_t
getsamples__FORWARD_CONTROL__position_reached (void)
{
    // Take as many samples as necessary. This shall be updated to verify the
    asserv_move_arm (1666, 100);
    return getsamples_next (FORWARD_CONTROL, position_reached);
}

/*
 * START =ok=>
 *  => GO_TO_POSITION
 *   Go to the distributor. The distributor to reach shall be setted in the fsm structure.
 */
fsm_branch_t
getsamples__START__ok (void)
{
    asserv_set_x_position (getsamples_data.distributor_x - 100);
    asserv_set_y_position (getsamples_data.distributor_y - 100);
    return getsamples_next (START, ok);
}

/*
 * TAKE_SAMPLES =sample_took=>
 * no_more => BACKWARD
 *   If the quantity of samples are tooked, then go backeward and conitnue classifying the samples.
 * more => TAKE_SAMPLES
 *   Continue to take samples and classify the next sample.
 */
fsm_branch_t
getsamples__TAKE_SAMPLES__sample_took (void)
{
    // Decrement the samples counter.
    if (getsamples_data.samples)
      {
	asserv_move_arm (1666, 100);
	return getsamples_next_branch (TAKE_SAMPLES, sample_took, more);
      }
    else
      {
	// Try to end the position to the distributor.
	asserv_set_x_position (getsamples_data.distributor_x - 20);
	// Go to the color distributor.
	asserv_set_y_position (getsamples_data.distributor_y - 20);

	return getsamples_next_branch (TAKE_SAMPLES, sample_took, no_more);
      }
}

/*
 * BACKWARD =position_reached=>
 *  => END
 *   Ending this state machine.
 */
fsm_branch_t
getsamples__BACKWARD__position_reached (void)
{
    asserv_move_arm (5000, 100);
    return getsamples_next (BACKWARD, position_reached);
}

/*
 * GO_TO_POSITION =position_failed=>
 *  => GO_TO_POSITION
 *   Go to another point before trying to go to this one again.
 */
fsm_branch_t
getsamples__GO_TO_POSITION__position_failed (void)
{
    // TODO In this case i don't know what to do.
    return getsamples_next (GO_TO_POSITION, position_failed);
}

/*
 * GO_TO_POSITION =position_reached=>
 *  => PREPARE_ARM
 *   Go to the position desired, it is very near the position of the distributor in case it is a ice distributor or sample distributor.
 */
fsm_branch_t
getsamples__GO_TO_POSITION__position_reached (void)
{
    // Put the ARM in the position to allow the robot to take samples from the
    // distributor.
    asserv_move_arm (625, 100);
    return getsamples_next (GO_TO_POSITION, position_reached);
}


