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

/* Positions when waiting a puck*/
uint16_t posx[4] =
{
    0 * ELEVATOR_MM_TO_STEP,
    25 * ELEVATOR_MM_TO_STEP,
    55 * ELEVATOR_MM_TO_STEP,
    85 * ELEVATOR_MM_TO_STEP
};
/* Positions when we go to a target zone */
uint16_t posy[3] =
{
    125 * ELEVATOR_MM_TO_STEP,
    95 * ELEVATOR_MM_TO_STEP,
    65 * ELEVATOR_MM_TO_STEP
};

/* nb puck on the elevator */
uint8_t nb_puck_elvt = 0;

/* increase/decrease of pos y */
#define MAJ_POSY 100
#define MIN_POSY 50

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
 *   make initializations (elevator zero and open doors)
 */
fsm_branch_t
elevator__WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    pwm_set(OPEN_DOOR_PWM, 0);
    asserv_elevator_zero_position();
    return elevator_next (WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * INIT =doors_opened=>
 *  => GO_TO_POS_X
 *   make initializations (close doors)
 */
fsm_branch_t
elevator__INIT__doors_opened (void)
{
    elevator_is_ready = 0;
    pwm_set(CLOSE_DOOR_PWM, TIME_DOORS_PWM);
    asserv_move_elevator_absolute(posx[nb_puck_in_elvt],
				  ASSERV_ELVT_SPEED_DEFAULT);
    return elevator_next (INIT, doors_opened);
}

/*
 * GO_TO_POS_X =in_position=>
 *  => WAIT_A_PUCK
 *   in position and ready to get a new puck
 */
fsm_branch_t
elevator__GO_TO_POS_X__in_position (void)
{
    elevator_is_ready = 1;
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
    elevator_is_ready = 0;
    elvt_new_puck = 0;
    // TODO time_ok
    if(nb_puck_elvt < 4)
      {
/*	&&
       ((chrono_remaining_time() - OK_TIME_LIMIT > 0)
       || nb_puck_fb != 0))
       */
	asserv_move_elevator_absolute(posx[nb_puck_in_elvt],
				      ASSERV_ELVT_SPEED_DEFAULT);
	return elevator_next_branch (WAIT_A_PUCK, new_puck, ok_for_other_pucks);
      }
    else
	return elevator_next_branch (WAIT_A_PUCK, new_puck, not_ok_for_other_pucks);
}

/*
 * TODO time_up
 * WAIT_A_PUCK =time_up=>
 *  => WAIT_POS_ORDER
 *   no more time to wait a new puck
 */
fsm_branch_t
elevator__WAIT_A_PUCK__time_up (void)
{
    elevator_is_ready = 0;
    return elevator_next (WAIT_A_PUCK, time_up);
}

/*
 * WAIT_POS_ORDER =order_received=>
 *  => GO_TO_POS_Y
 *   go to position Y
 */
fsm_branch_t
elevator__WAIT_POS_ORDER__order_received (void)
{
    asserv_move_elevator_absolute(posy[elvt_order] + MAJ_POSY,
				  ASSERV_ELVT_SPEED_DEFAULT);
    elvt_order = 0;
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
    asserv_move_elevator_absolute(posy[elvt_order] - MIN_POSY,
				  ASSERV_ELVT_SPEED_DEFAULT);
    elvt_order = 0;
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
    pwm_set(OPEN_DOOR_PWM, TIME_DOORS_PWM);
    return elevator_next (LAND_ELEVATOR, in_position);
}

/*
 * MINI_CLOSE =door_move_finished=>
 *  => OPEN_DOORS
 *   try to release pucks again
 */
fsm_branch_t
elevator__MINI_CLOSE__door_move_finished (void)
{
    pwm_set(OPEN_DOOR_PWM, TIME_DOORS_PWM);
    return elevator_next (MINI_CLOSE, door_move_finished);
}

/*
 * OPEN_DOORS =doors_opened=>
 *  => WAIT_FOR_CLOSE_ORDER
 *   wait for close order
 */
fsm_branch_t
elevator__OPEN_DOORS__doors_opened (void)
{
    nb_puck_elvt = 0;
    nb_puck_in_elvt = 0;
    return elevator_next (OPEN_DOORS, doors_opened);
}

/*
 * OPEN_DOORS =door_move_finished=>
 *  => MINI_CLOSE
 *   try to unblock doors
 */
fsm_branch_t
elevator__OPEN_DOORS__door_move_finished (void)
{
    pwm_set(CLOSE_DOOR_PWM, TIME_LIGHT_DOORS_PWM);
    return elevator_next (OPEN_DOORS, door_move_finished);
}

/*
 * WAIT_FOR_CLOSE_ORDER =order_received=>
 *  => CLOSE_DOORS
 *   closing doors
 */
fsm_branch_t
elevator__WAIT_FOR_CLOSE_ORDER__order_received (void)
{
    pwm_set(CLOSE_DOOR_PWM, TIME_DOORS_PWM);
    return elevator_next (WAIT_FOR_CLOSE_ORDER, order_received);
}

/*
 * CLOSE_DOORS =door_move_finished=>
 *  => GO_TO_POS_X
 *   pucks are released and elevator is ready to make a new column
 */
fsm_branch_t
elevator__CLOSE_DOORS__door_move_finished (void)
{
    asserv_move_elevator_absolute(posx[nb_puck_in_elvt],
				  ASSERV_ELVT_SPEED_DEFAULT);
    return elevator_next (CLOSE_DOORS, door_move_finished);
}


