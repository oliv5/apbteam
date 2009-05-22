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
#include "elevator.h"
#include "asserv.h"
#include "pwm.h"
#include "chrono.h"
#include "filterbridge.h"
#include "top.h"
#include "modules/trace/trace.h"
#include "trace_event.h"
#include "cylinder.h"

/* Positions when waiting a puck*/
uint16_t posx[4] =
{
    3 * ELEVATOR_MM_TO_STEP,
    20 * ELEVATOR_MM_TO_STEP,
    50 * ELEVATOR_MM_TO_STEP,
    80 * ELEVATOR_MM_TO_STEP
};
/* Positions when we go to a target zone */
uint16_t posy[3] =
{
    135 * ELEVATOR_MM_TO_STEP,
    105 * ELEVATOR_MM_TO_STEP,
    75 * ELEVATOR_MM_TO_STEP
};

/* increase/decrease of pos y */
#define MAJ_POSY (10 * ELEVATOR_MM_TO_STEP)
#define MIN_POSY (5 * ELEVATOR_MM_TO_STEP)

/* time limit */
/* TODO to be define */
#define OK_TIME_LIMIT 20

/*
 * IDLE =start=>
 *  => WAIT_JACK_IN
 *   waiting for jack
 */
fsm_branch_t
elevator__IDLE__start (void)
{
    return elevator_next (IDLE, start);
}

/*
 * WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => INIT
 *   open doors.
 *   find the zero of the elevator.
 */
fsm_branch_t
elevator__WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    /* Open doors. */
    pwm_set (OPEN_DOOR_PWM, 0);
    /* Find the zero of the elevator. */
    asserv_elevator_zero_position ();
    return elevator_next (WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * INIT =doors_opened=>
 *  => GO_TO_POS_X
 *   doors opened, close doors.
 */
fsm_branch_t
elevator__INIT__doors_opened (void)
{
    /* FIXME: why this is here? */
    elvt_is_ready = 0;
    /* Close the door. */
    pwm_set (CLOSE_DOOR_PWM, 2*TIME_DOORS_PWM);
    return elevator_next (INIT, doors_opened);
}

/*
 * GO_TO_POS_X =in_position=>
 *  => WAIT_A_PUCK
 *   elevator in position zero, move to first position.
 */
fsm_branch_t
elevator__GO_TO_POS_X__in_position (void)
{
    /* FIXME: this sucks, look at elevator.fsm for a real fix. */
    elvt_is_ready = 1;
    /* move to first position. */
    asserv_move_elevator_absolute(posx[elvt_nb_puck],
				  ASSERV_ELVT_SPEED_DEFAULT);
    return elevator_next (GO_TO_POS_X, in_position);
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
    elvt_is_ready = 0;
    elvt_new_puck = 0;
    // TODO time_ok
    if(elvt_nb_puck < 4)
      {
/*	&&
       ((chrono_remaining_time() - OK_TIME_LIMIT > 0)
       || nb_puck_fb != 0))
       */
	asserv_move_elevator_absolute(posx[elvt_nb_puck],
				      ASSERV_ELVT_SPEED_DEFAULT);
	return elevator_next_branch (WAIT_A_PUCK, new_puck, ok_for_other_pucks);
      }
    else
	return elevator_next_branch (WAIT_A_PUCK, new_puck, not_ok_for_other_pucks);
}

/*
 * WAIT_A_PUCK =order_bypass=>
 *  => WAIT_BRIDGE_EMPTY
 *   elevator filling has been shut, get ready to drop pucks
 */
fsm_branch_t
elevator__WAIT_A_PUCK__order_bypass (void)
{
    return elevator_next (WAIT_A_PUCK, order_bypass);
}

/*
 * WAIT_BRIDGE_EMPTY =bridge_empty=>
 *  => GO_TO_POS_Y
 *   bridge is empty, we are happy, we drop the column
 */
fsm_branch_t
elevator__WAIT_BRIDGE_EMPTY__bridge_empty (void)
{
    elvt_is_ready = 0;
    asserv_move_elevator_absolute(posy[elvt_position_required - 1] - MAJ_POSY,
				  ASSERV_ELVT_SPEED_DEFAULT);
    return elevator_next (WAIT_BRIDGE_EMPTY, bridge_empty);
}

/*
 * WAIT_BRIDGE_EMPTY =state_timeout=>
 *  => GO_TO_POS_Y
 *   we suppose bridge is empty and we said it to the fsm
 */
fsm_branch_t
elevator__WAIT_BRIDGE_EMPTY__state_timeout (void)
{
    elvt_is_ready = 0;
    /* XXX bridge is empty but he don't know it yet */
    fb_nb_puck = 0;
    asserv_move_elevator_absolute(posy[elvt_position_required - 1] - MAJ_POSY,
				  ASSERV_ELVT_SPEED_DEFAULT);
    TRACE (TRACE_FSM__NBPUCKS, top_total_puck_taken, top_puck_inside_bot,
			      cylinder_nb_puck, fb_nb_puck, elvt_nb_puck);
    return elevator_next (WAIT_BRIDGE_EMPTY, state_timeout);
}

/*
 * WAIT_BRIDGE_EMPTY =new_puck=>
 *  => WAIT_A_PUCK
 *   We have a new puck, elevator must move, we go to WAIT_A_PUCK
 */
fsm_branch_t
elevator__WAIT_BRIDGE_EMPTY__new_puck (void)
{
    return elevator_next (WAIT_BRIDGE_EMPTY, new_puck);
}

/*
 * WAIT_POS_ORDER =order_received=>
 *  => GO_TO_POS_Y
 *   go to position Y
 */
fsm_branch_t
elevator__WAIT_POS_ORDER__order_received (void)
{
    asserv_move_elevator_absolute(posy[elvt_position_required - 1] - MAJ_POSY,
				  ASSERV_ELVT_SPEED_DEFAULT);
    return elevator_next (WAIT_POS_ORDER, order_received);
}

/*
 * GO_TO_POS_Y =in_position=>
 *  => WAIT_FOR_RELEASE_ORDER
 *   ready to release pucks at altitude Y
 */
fsm_branch_t
elevator__GO_TO_POS_Y__in_position (void)
{
    elvt_order_in_progress = 0;
    return elevator_next (GO_TO_POS_Y, in_position);
}

/*
 * WAIT_FOR_RELEASE_ORDER =order_received=>
 *  => LAND_ELEVATOR
 *   make the elevator touch the target zone
 */
fsm_branch_t
elevator__WAIT_FOR_RELEASE_ORDER__order_received (void)
{
    uint16_t true_pos = elvt_degraded_mode?posy[elvt_position_required - 1]:
	posy[elvt_position_required - 1] + MIN_POSY;
    asserv_move_elevator_absolute(true_pos, ASSERV_ELVT_SPEED_DEFAULT);
    return elevator_next (WAIT_FOR_RELEASE_ORDER, order_received);
}

/*
 * LAND_ELEVATOR =in_position=>
 *  => OPEN_DOORS
 *   release pucks to the target position (I hope)
 */
fsm_branch_t
elevator__LAND_ELEVATOR__in_position (void)
{
    pwm_set(OPEN_DOOR_PWM, 0);
    return elevator_next (LAND_ELEVATOR, in_position);
}

/*
 * MINI_CLOSE =state_timeout=>
 *  => OPEN_DOORS
 *   try to release pucks again
 */
fsm_branch_t
elevator__MINI_CLOSE__state_timeout (void)
{
    pwm_set(OPEN_DOOR_PWM, 0);
    return elevator_next (MINI_CLOSE, state_timeout);
}

/*
 * OPEN_DOORS =doors_opened=>
 *  => WAIT_FOR_CLOSE_ORDER
 *   wait for close order
 */
fsm_branch_t
elevator__OPEN_DOORS__doors_opened (void)
{
    top_puck_inside_bot -= elvt_nb_puck;
    elvt_nb_puck = 0;
    pwm_set(0,0);
    elvt_order_in_progress = 0;
    TRACE (TRACE_FSM__NBPUCKS, top_total_puck_taken, top_puck_inside_bot,
			      cylinder_nb_puck, fb_nb_puck, elvt_nb_puck);
    return elevator_next (OPEN_DOORS, doors_opened);
}

/*
 * OPEN_DOORS =state_timeout=>
 *  => MINI_CLOSE
 *   try to unblock doors
 */
fsm_branch_t
elevator__OPEN_DOORS__state_timeout (void)
{
    pwm_set(CLOSE_DOOR_PWM,0);
    return elevator_next (OPEN_DOORS, state_timeout);
}

/*
 * WAIT_FOR_CLOSE_ORDER =order_received=>
 *  => CLOSE_DOORS
 *   closing doors
 */
fsm_branch_t
elevator__WAIT_FOR_CLOSE_ORDER__order_received (void)
{
    pwm_set(CLOSE_DOOR_PWM, 0);
    return elevator_next (WAIT_FOR_CLOSE_ORDER, order_received);
}

/*
 * CLOSE_DOORS =state_timeout=>
 *  => GO_TO_POS_X
 *   pucks are released and elevator is ready to make a new column
 */
fsm_branch_t
elevator__CLOSE_DOORS__state_timeout (void)
{
    elvt_order_in_progress = 0;
    pwm_set(0,0);
    asserv_move_elevator_absolute(posx[elvt_nb_puck],
				  ASSERV_ELVT_SPEED_DEFAULT);
    return elevator_next (CLOSE_DOORS, state_timeout);
}
