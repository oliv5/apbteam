/* init_cb.c - init FSM callbacks. */
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
#include "init_cb.h"
#include "asserv.h"
#include "init.h"
#include "playground.h"
#include "main.h"
#include "aquajim.h"
#include "switch.h"
#include "modules/trace/trace.h"

/*
 * IDLE =start=>
 *  => WAIT_JACK_IN
 *   wait for the jack to be inserted into the bot
 */
fsm_branch_t
init__IDLE__start (void)
{
    return init_next (IDLE, start);
}

/*
 * WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => WAIT_2_SEC
 *   wait for the operator hand disappears
 */
fsm_branch_t
init__WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    /* Get the color. */
    bot_color = switch_get_color ();
    /* Initialize trace module (erase the flash). */
    trace_init ();
    return init_next (WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * WAIT_2_SEC =state_timeout=>
 *  => GOTO_THE_WALL
 *   go to the first wall
 */
fsm_branch_t
init__WAIT_2_SEC__state_timeout (void)
{
    /* Move forward to the wall. */
    asserv_go_to_the_wall (0);
    return init_next (WAIT_2_SEC, state_timeout);
}

/*
 * GOTO_THE_WALL =move_done=>
 *  => GO_BACKWARD
 *   go backward for INIT_DIST millimeters
 */
fsm_branch_t
init__GOTO_THE_WALL__move_done (void)
{
    asserv_move_linearly (-INIT_DIST);
    return init_next (GOTO_THE_WALL, move_done);
}

/*
 * GO_BACKWARD =move_done=>
 *  => TURN_90_DEGREES_CCW
 *   turn bot for 90 degrees counterclockwise
 */
fsm_branch_t
init__GO_BACKWARD__move_done (void)
{
    asserv_goto_angle (PG_A_VALUE_COMPUTING (90 * BOT_ANGLE_DEGREE));
    return init_next (GO_BACKWARD, move_done);
}

/*
 * TURN_90_DEGREES_CCW =move_done=>
 *  => GOTO_THE_WALL_AGAIN
 *   go to the wall for the second time
 */
fsm_branch_t
init__TURN_90_DEGREES_CCW__move_done (void)
{
    asserv_go_to_the_wall (0);
    return init_next (TURN_90_DEGREES_CCW, move_done);
}

/*
 * GOTO_THE_WALL_AGAIN =move_done=>
 *  => GO_BACKWARD_AGAIN
 *   go backward for INIT_DIST millimeters again
 */
fsm_branch_t
init__GOTO_THE_WALL_AGAIN__move_done (void)
{
    asserv_move_linearly (-INIT_DIST);
    return init_next (GOTO_THE_WALL_AGAIN, move_done);
}

/*
 * GO_BACKWARD_AGAIN =move_done=>
 *  => SET_POSITION
 *   set real position to asserv
 */
fsm_branch_t
init__GO_BACKWARD_AGAIN__move_done (void)
{
    asserv_set_position (PG_X_VALUE_COMPUTING (BOT_LENGTH / 2 + INIT_DIST),
			 PG_HEIGHT - (BOT_WIDTH / 2 - INIT_DIST),
			 PG_A_VALUE_COMPUTING (180 * BOT_ANGLE_DEGREE));
    return init_next (GO_BACKWARD_AGAIN, move_done);
}

/*
 * SET_POSITION =asserv_last_cmd_ack=>
 *  => IDLE
 *   turn bot for 180 degrees counterclockwise
 */
fsm_branch_t
init__SET_POSITION__asserv_last_cmd_ack (void)
{
    asserv_goto_angle (PG_A_VALUE_COMPUTING (0 * BOT_ANGLE_DEGREE));
    return init_next (SET_POSITION, asserv_last_cmd_ack);
}


