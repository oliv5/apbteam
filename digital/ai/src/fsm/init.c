/* init.c */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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

#include "asserv.h"
#include "contact.h"
#include "chrono.h"

#define FSM_NAME AI
#include "fsm.h"
#include "fsm_queue.h"

#include "bot.h"
#include "init_defs.h"

/*
 * Initialise robot position with a calibration procedure.
 */

FSM_STATES (
	    /* Initial state, waiting for jack. */
	    INIT_START,
	    /* After first jack insertion, waiting for removal to initialise actuators. */
	    INIT_WAITING_FIRST_JACK_OUT,
	    /* Initialising actuators, then waiting for second jack insertion. */
	    INIT_INITIALISING_ACTUATORS,
	    /* After second jack insertion, waiting the operator remove its
	     * hand. */
	    INIT_WAITING_HANDS_OUT,
	    /* Finding the first wall. */
	    INIT_FINDING_FIRST_WALL,
	    /* Going away from the wall. */
	    INIT_GOING_AWAY_FIRST_WALL,
	    /* Turning to face the other wall. */
	    INIT_FACING_SECOND_WALL,
	    /* Waiting after rotation for robot to stabilize. */
	    INIT_WAITING_AFTER_FACING_SECOND_WALL,
	    /* Finding the second wall. */
	    INIT_FINDING_SECOND_WALL,
	    /* Going away from the wall. */
	    INIT_GOING_AWAY_SECOND_WALL,
#ifdef INIT_START_POSITION_ANGLE
	    /* Facing the start position. */
	    INIT_FACING_START_POSITION,
#endif
	    /* Going to start position. */
	    INIT_GOING_TO_START_POSITION,
	    /* Waiting for the round start (waiting for the jack). */
	    INIT_WAITING_SECOND_JACK_OUT,
	    /* Initialisation finished, nothing else to do. */
	    INIT_FINISHED)

FSM_EVENTS (
	    /* Jack is inserted. */
	    jack_inserted,
	    /* Jack is removed. */
	    jack_removed,
	    /* Sent to initialise actuators. */
	    init_actuators,
	    /* Sent to start round. */
	    init_start_round)

FSM_START_WITH (INIT_START)

FSM_TRANS (INIT_START, jack_inserted, INIT_WAITING_FIRST_JACK_OUT)
{
    return FSM_NEXT (INIT_START, jack_inserted);
}

FSM_TRANS (INIT_WAITING_FIRST_JACK_OUT, jack_removed,
	   INIT_INITIALISING_ACTUATORS)
{
    fsm_queue_post_event (FSM_EVENT (AI, init_actuators));
    return FSM_NEXT (INIT_WAITING_FIRST_JACK_OUT, jack_removed);
}

FSM_TRANS (INIT_INITIALISING_ACTUATORS, jack_inserted, INIT_WAITING_HANDS_OUT)
{
    team_color = contact_get_color ();
    return FSM_NEXT (INIT_INITIALISING_ACTUATORS, jack_inserted);
}

FSM_TRANS_TIMEOUT (INIT_WAITING_HANDS_OUT, 225, INIT_FINDING_FIRST_WALL)
{
    asserv_set_speed (BOT_SPEED_INIT);
    asserv_push_the_wall (INIT_FIRST_WALL_PUSH);
    return FSM_NEXT_TIMEOUT (INIT_WAITING_HANDS_OUT);
}

FSM_TRANS (INIT_FINDING_FIRST_WALL, robot_move_success,
	   INIT_GOING_AWAY_FIRST_WALL)
{
    asserv_move_linearly (INIT_FIRST_WALL_AWAY);
    return FSM_NEXT (INIT_FINDING_FIRST_WALL, robot_move_success);
}

FSM_TRANS (INIT_GOING_AWAY_FIRST_WALL, robot_move_success,
	   INIT_FACING_SECOND_WALL)
{
    asserv_goto_angle (INIT_SECOND_WALL_ANGLE);
    return FSM_NEXT (INIT_GOING_AWAY_FIRST_WALL, robot_move_success);
}

FSM_TRANS (INIT_FACING_SECOND_WALL, robot_move_success,
	   INIT_WAITING_AFTER_FACING_SECOND_WALL)
{
    return FSM_NEXT (INIT_FACING_SECOND_WALL, robot_move_success);
}

FSM_TRANS_TIMEOUT (INIT_WAITING_AFTER_FACING_SECOND_WALL, 225 / 2,
		   INIT_FINDING_SECOND_WALL)
{
    asserv_push_the_wall (INIT_SECOND_WALL_PUSH);
    return FSM_NEXT_TIMEOUT (INIT_WAITING_AFTER_FACING_SECOND_WALL);
}

FSM_TRANS (INIT_FINDING_SECOND_WALL, robot_move_success,
	   INIT_GOING_AWAY_SECOND_WALL)
{
    asserv_move_linearly (INIT_SECOND_WALL_AWAY);
    return FSM_NEXT (INIT_FINDING_SECOND_WALL, robot_move_success);
}

#ifdef INIT_START_POSITION_ANGLE
FSM_TRANS (INIT_GOING_AWAY_SECOND_WALL, robot_move_success,
	   INIT_FACING_START_POSITION)
{
    asserv_goto_angle (INIT_START_POSITION_ANGLE);
    return FSM_NEXT (INIT_GOING_AWAY_SECOND_WALL, robot_move_success);
}

FSM_TRANS (INIT_FACING_START_POSITION, robot_move_success,
	   INIT_GOING_TO_START_POSITION)
{
    asserv_goto_xya (INIT_START_POSITION);
    return FSM_NEXT (INIT_FACING_START_POSITION, robot_move_success);
}

#else

FSM_TRANS (INIT_GOING_AWAY_SECOND_WALL, robot_move_success,
	   INIT_GOING_TO_START_POSITION)
{
    asserv_goto_xya (INIT_START_POSITION);
    return FSM_NEXT (INIT_GOING_AWAY_SECOND_WALL, robot_move_success);
}

#endif

FSM_TRANS (INIT_GOING_TO_START_POSITION, robot_move_success,
	   INIT_WAITING_SECOND_JACK_OUT)
{
    asserv_set_speed (BOT_SPEED_NORMAL);
    return FSM_NEXT (INIT_GOING_TO_START_POSITION, robot_move_success);
}

FSM_TRANS (INIT_WAITING_SECOND_JACK_OUT, jack_removed, INIT_FINISHED)
{
    chrono_init ();
    fsm_queue_post_event (FSM_EVENT (AI, init_start_round));
    return FSM_NEXT (INIT_WAITING_SECOND_JACK_OUT, jack_removed);
}

