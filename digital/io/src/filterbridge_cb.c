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
 * CLOSE_FIRST_DOOR =first_door_closed=>
 *  => WAIT_RGB_IDENT
 *   get puck color
 */
fsm_branch_t
filterbridge__CLOSE_FIRST_DOOR__first_door_closed (void)
{
    return filterbridge_next (CLOSE_FIRST_DOOR, first_door_closed);
}

/*
 * WAIT_A_PUCK =puck_on_pos2=>
 *  => CLOSE_FIRST_DOOR
 *   close the first door after a puck is ready for filtering
 */
fsm_branch_t
filterbridge__WAIT_A_PUCK__puck_on_pos2 (void)
{
    return filterbridge_next (WAIT_A_PUCK, puck_on_pos2);
}

/*
 * CLOSE_SECOND_DOOR =second_door_closed=>
 *  => OPEN_FIRST_DOOR
 *   filter bridge is ready to get a new puck
 */
fsm_branch_t
filterbridge__CLOSE_SECOND_DOOR__second_door_closed (void)
{
    return filterbridge_next (CLOSE_SECOND_DOOR, second_door_closed);
}

/*
 * WAIT_RGB_IDENT =good_color=>
 *  => OPEN_SECOND_DOOR
 *   put puck to the lift
 */
fsm_branch_t
filterbridge__WAIT_RGB_IDENT__good_color (void)
{
    return filterbridge_next (WAIT_RGB_IDENT, good_color);
}

/*
 * WAIT_RGB_IDENT =bad_color=>
 *  => EJECT_PUCK
 *   eject bad PUCK
 */
fsm_branch_t
filterbridge__WAIT_RGB_IDENT__bad_color (void)
{
    return filterbridge_next (WAIT_RGB_IDENT, bad_color);
}

/*
 * BLAH =no_puck_on_pos2=>
 *  => OPEN_FIRST_DOOR
 *   make bridge ready to test a new puck
 */
fsm_branch_t
filterbridge__BLAH__no_puck_on_pos2 (void)
{
    return filterbridge_next (BLAH, no_puck_on_pos2);
}

/*
 * BLAH =puck_on_pos2=>
 *  => EJECT_PUCK
 *   re-eject this sticky puck, grml!
 */
fsm_branch_t
filterbridge__BLAH__puck_on_pos2 (void)
{
    return filterbridge_next (BLAH, puck_on_pos2);
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
 * RETURN_NORMAL_POS =bridge_in_position=>
 *  => BLAH
 *   ready for a new puck test (unless bad puck is here yet)
 */
fsm_branch_t
filterbridge__RETURN_NORMAL_POS__bridge_in_position (void)
{
    return filterbridge_next (RETURN_NORMAL_POS, bridge_in_position);
}

/*
 * OPEN_FIRST_DOOR =first_door_opened=>
 *  => IDLE
 *   filter bridge ready
 */
fsm_branch_t
filterbridge__OPEN_FIRST_DOOR__first_door_opened (void)
{
    return filterbridge_next (OPEN_FIRST_DOOR, first_door_opened);
}

/*
 * OPEN_SECOND_DOOR =no_puck_on_pos2=>
 *  => CLOSE_SECOND_DOOR
 *   close the lift access door and tell to lift it has a new puck
 */
fsm_branch_t
filterbridge__OPEN_SECOND_DOOR__no_puck_on_pos2 (void)
{
    return filterbridge_next (OPEN_SECOND_DOOR, no_puck_on_pos2);
}

/*
 * EJECT_PUCK =puck_ejected=>
 *  => RETURN_NORMAL_POS
 *   put bridge on normal position after puck ejection
 */
fsm_branch_t
filterbridge__EJECT_PUCK__puck_ejected (void)
{
    return filterbridge_next (EJECT_PUCK, puck_ejected);
}


