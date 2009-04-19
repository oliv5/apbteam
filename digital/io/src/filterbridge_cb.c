/* filterbridge_cb.c - filterbridge FSM callbacks. */
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
#include "filterbridge_cb.h"

/*
 * WAIT_A_PUCK =puck_on_pos2=>
 * nb_puck_ok => WAIT_RGB_PROBE
 *   probe color of the new puck
 * too_much_puck => EJECT_PUCK
 *   we have too much puck, eject the puck quickly and whistle
 */
fsm_branch_t
filterbridge__WAIT_A_PUCK__puck_on_pos2 (void)
{
    return filterbridge_next_branch (WAIT_A_PUCK, puck_on_pos2, nb_puck_ok);
    return filterbridge_next_branch (WAIT_A_PUCK, puck_on_pos2, too_much_puck);
}

/*
 * WAIT_RGB_PROBE =color_probed=>
 * bad_color => EJECT_PUCK
 *   eject bad puck
 * good_color => OPEN_DOOR
 *   put puck to the lift
 */
fsm_branch_t
filterbridge__WAIT_RGB_PROBE__color_probed (void)
{
    return filterbridge_next_branch (WAIT_RGB_PROBE, color_probed, bad_color);
    return filterbridge_next_branch (WAIT_RGB_PROBE, color_probed, good_color);
}

/*
 * IDLE =lift_ready=>
 *  => WAIT_A_PUCK
 *   the lift is ready to get pucks, we can begin testing procedure
 */
fsm_branch_t
filterbridge__IDLE__lift_ready (void)
{
    return filterbridge_next (IDLE, lift_ready);
}

/*
 * CLOSE_DOOR =door_closed=>
 *  => IDLE
 *   filter bridge ready
 */
fsm_branch_t
filterbridge__CLOSE_DOOR__door_closed (void)
{
    return filterbridge_next (CLOSE_DOOR, door_closed);
}

/*
 * RETURN_NORMAL_POS =bridge_in_position=>
 *  => CLOSE_DOOR
 *   make bridge ready to test a new puck
 */
fsm_branch_t
filterbridge__RETURN_NORMAL_POS__bridge_in_position (void)
{
    return filterbridge_next (RETURN_NORMAL_POS, bridge_in_position);
}

/*
 * OPEN_DOOR =no_puck_on_pos2=>
 *  => CLOSE_DOOR
 *   release puck to the lift
 */
fsm_branch_t
filterbridge__OPEN_DOOR__no_puck_on_pos2 (void)
{
    return filterbridge_next (OPEN_DOOR, no_puck_on_pos2);
}

/*
 * EJECT_PUCK =ejection_done=>
 *  => RETURN_NORMAL_POS
 *   put bridge on normal position after puck ejection
 */
fsm_branch_t
filterbridge__EJECT_PUCK__ejection_done (void)
{
    return filterbridge_next (EJECT_PUCK, ejection_done);
}
