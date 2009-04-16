/* elevator_cb.c - elevator FSM callbacks. */
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
#include "elevator_cb.h"

/*
 * OPEN_DOORS =doors_opened=>
 *  => WAIT_FOR_CLOSE_ORDER
 *   wait for close order
 */
fsm_branch_t
elevator__OPEN_DOORS__doors_opened (void)
{
    return elevator_next (OPEN_DOORS, doors_opened);
}

/*
 * WAIT_FOR_CLOSE_ORDER =order_received=>
 *  => CLOSE_DOORS
 *   closing doors
 */
fsm_branch_t
elevator__WAIT_FOR_CLOSE_ORDER__order_received (void)
{
    return elevator_next (WAIT_FOR_CLOSE_ORDER, order_received);
}

/*
 * WAIT_A_PUCK =time_up=>
 *  => WAIT_POS_ORDER
 *   no more time to wait a new puck
 */
fsm_branch_t
elevator__WAIT_A_PUCK__time_up (void)
{
    return elevator_next (WAIT_A_PUCK, time_up);
}

/*
 * WAIT_A_PUCK =new_puck=>
 * ok_for_other_pucks => GO_TO_POS_X
 *   incrementing nb_puck var
 *   update elevator position to get a new puck
 * not_ok_for_other_pucks => WAIT_POS_ORDER
 *   incrementing nb_puck var
 *   no more time to wait a new puck
 */
fsm_branch_t
elevator__WAIT_A_PUCK__new_puck (void)
{
    return elevator_next_branch (WAIT_A_PUCK, new_puck, ok_for_other_pucks);
    return elevator_next_branch (WAIT_A_PUCK, new_puck, not_ok_for_other_pucks);
}

/*
 * GO_TO_POS_Y =in_position=>
 *  => WAIT_FOR_RELEASE_ORDER
 *   ready to release pucks at altitude Y
 */
fsm_branch_t
elevator__GO_TO_POS_Y__in_position (void)
{
    return elevator_next (GO_TO_POS_Y, in_position);
}

/*
 * GO_TO_POS_X =in_position=>
 *  => WAIT_A_PUCK
 *   in position and ready to get a new puck
 */
fsm_branch_t
elevator__GO_TO_POS_X__in_position (void)
{
    return elevator_next (GO_TO_POS_X, in_position);
}

/*
 * IDLE =started=>
 *  => GO_TO_POS_X
 *   match begin, we're going to be ready to get a new puck
 */
fsm_branch_t
elevator__IDLE__started (void)
{
    return elevator_next (IDLE, started);
}

/*
 * WAIT_FOR_RELEASE_ORDER =order_received=>
 *  => OPEN_DOORS
 *   release pucks to the target position (I hope)
 */
fsm_branch_t
elevator__WAIT_FOR_RELEASE_ORDER__order_received (void)
{
    return elevator_next (WAIT_FOR_RELEASE_ORDER, order_received);
}

/*
 * WAIT_POS_ORDER =order_received=>
 *  => GO_TO_POS_Y
 *   go to position Y
 */
fsm_branch_t
elevator__WAIT_POS_ORDER__order_received (void)
{
    return elevator_next (WAIT_POS_ORDER, order_received);
}

/*
 * CLOSE_DOORS =doors_closed=>
 *  => GO_TO_POS_X
 *   pucks are released and elevator is ready to make a new column
 */
fsm_branch_t
elevator__CLOSE_DOORS__doors_closed (void)
{
    return elevator_next (CLOSE_DOORS, doors_closed);
}


