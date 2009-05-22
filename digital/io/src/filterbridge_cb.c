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
#include "elevator.h"
#include "servo_pos.h"
#include "aquajim.h"
#include "filterbridge.h"
#include "top.h"
#include "cylinder.h"

/*
 * IDLE =start=>
 *  => WAIT_JACK_IN
 *   we wait for the jack
 */
fsm_branch_t
filterbridge__IDLE__start (void)
{
    return filterbridge_next (IDLE, start);
}

/*
 * WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => CLOSE_DOOR
 *   we init the bridge
 */
fsm_branch_t
filterbridge__WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    servo_pos_move_to(SERVO_FINGER_ID, SERVO_FINGER_IDLE);
    servo_pos_move_to(SERVO_DOOR_ID, SERVO_DOOR_CLOSE);
    return filterbridge_next (WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * CLOSE_DOOR =state_timeout=>
 *  => WAIT_A_PUCK
 *   we are ready, we wait a puck
 */
fsm_branch_t
filterbridge__CLOSE_DOOR__state_timeout (void)
{
    return filterbridge_next (CLOSE_DOOR, state_timeout);
}

/*
 * WAIT_A_PUCK =puck_on_pos2=>
 * lift_not_ready => WAIT_A_PUCK
 *   Lift not ready, we stand by
 * lift_ready => MARCEL_WAIT
 *   wait the puck is entirely on pos2
 */
fsm_branch_t
filterbridge__WAIT_A_PUCK__puck_on_pos2 (void)
{
    if(elvt_is_ready)
      {
	return filterbridge_next_branch (WAIT_A_PUCK, puck_on_pos2, lift_ready);
      }
    else
	return filterbridge_next_branch (WAIT_A_PUCK, puck_on_pos2, lift_not_ready);
}

/*
 * MARCEL_WAIT =state_timeout=>
 *  => OPEN_DOOR
 *   lift ready, we deliver
 */
fsm_branch_t
filterbridge__MARCEL_WAIT__state_timeout (void)
{
    servo_pos_move_to(SERVO_DOOR_ID, SERVO_DOOR_OPEN);
    return filterbridge_next (MARCEL_WAIT, state_timeout);
}

/*
 * OPEN_DOOR =state_timeout=>
 *  => PUSH_PUCK
 *   we push the puck to the lift
 */
fsm_branch_t
filterbridge__OPEN_DOOR__state_timeout (void)
{
    servo_pos_move_to(SERVO_FINGER_ID,SERVO_FINGER_PUSHING);
    return filterbridge_next (OPEN_DOOR, state_timeout);
}

/*
 * PUSH_PUCK =no_puck_on_pos2=>
 *  => TEMPO_ELVETATOR_COMMUNICATION
 *   nothing to do.
 */
fsm_branch_t
filterbridge__PUSH_PUCK__no_puck_on_pos2 (void)
{
    return filterbridge_next (PUSH_PUCK, no_puck_on_pos2);
}

/*
 * TEMPO_ELVETATOR_COMMUNICATION =state_timeout=>
 *  => CLOSE_DOOR
 *   the puck disappears, we close doors
 *   tell the elevator we are ready.
 */
fsm_branch_t
filterbridge__TEMPO_ELVETATOR_COMMUNICATION__state_timeout (void)
{
    ++elvt_nb_puck;
    if(fb_nb_puck) --fb_nb_puck;
    top_puck_inside_bot = cylinder_nb_puck + fb_nb_puck + elvt_nb_puck;
    elvt_new_puck = 1;
    servo_pos_move_to(SERVO_FINGER_ID, SERVO_FINGER_IDLE);
    servo_pos_move_to(SERVO_DOOR_ID, SERVO_DOOR_CLOSE);
    return filterbridge_next (TEMPO_ELVETATOR_COMMUNICATION, state_timeout);
}


