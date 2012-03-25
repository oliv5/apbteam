/* init.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
#define FSM_NAME AI

#include "common.h"
#include "fsm.h"
#include "asserv.h"
#include "init.h"
#include "playground_2010.h"
#include "main.h"
#include "bot.h"
#include "switch.h"
#include "chrono.h"
#include "modules/trace/trace.h"
#include "modules/path/path.h"

uint8_t init_match_is_started = 0;

FSM_STATES (
	    /* waiting for the beginning of the top FSM. */
	    INIT_IDLE,
	    /* wait for the jack to be inserted into the bot for the first time. */
	    INIT_WAIT_FIRST_JACK_IN,
	    /* wait the jack to be removed from the bot for the first time. */
	    INIT_WAIT_FIRST_JACK_OUT,
	    /* wait for the jack to be inserted into the bot for the second time. */
	    INIT_WAIT_SECOND_JACK_IN,
	    /* just wait for operator hands to be removed from the jack. */
	    INIT_WAIT_FOR_HANDS_OUT,
	    /* go to the wall for the first time. */
	    INIT_GO_TO_THE_WALL,
	    /* reset the Y position of the bot.
	     * reset the angular position of the bot.
	     * go backward from the wall for a specific distance. */
	    INIT_GO_AWAY_FROM_THE_WALL,
	    /* make the bot face the other wall. */
	    INIT_FACE_OTHER_WALL,
	    /* make sure rotation is really finished before continuing. */
	    INIT_WAIT_AFTER_ROTATION,
	    /* go to the wall for the second time. */
	    INIT_GO_TO_THE_WALL_AGAIN,
	    /* reset the X position of the bot.
	     * go backward from the wall for a specific distance. */
	    INIT_GO_AWAY_FROM_THE_WALL_AGAIN,
	    /* go to the start position. */
	    INIT_GO_TO_START_POSITION,
	    /* wait the jack to be removed from the bot for the second time. */
	    INIT_WAIT_SECOND_JACK_OUT)

FSM_EVENTS (
	    /* the jack is inserted into the bot. */
	    jack_inserted_into_bot,
	    /* the jack is removed from the bot. */
	    jack_removed_from_bot)

FSM_START_WITH (INIT_IDLE)

/*
 * do nothing.
 */
FSM_TRANS (INIT_IDLE, start, INIT_WAIT_FIRST_JACK_IN)
{
    return FSM_NEXT (INIT_IDLE, start);
}

/*
 * do nothing.
 */
FSM_TRANS (INIT_WAIT_FIRST_JACK_IN,
	   jack_inserted_into_bot,
	   INIT_WAIT_FIRST_JACK_OUT)
{
    return FSM_NEXT (INIT_WAIT_FIRST_JACK_IN, jack_inserted_into_bot);
}

/*
 * start trace module.
 * get and store the color of the bot.
 */
FSM_TRANS (INIT_WAIT_FIRST_JACK_OUT,
	   jack_removed_from_bot,
	   INIT_WAIT_SECOND_JACK_IN)
{
    /* Initialize trace module (erase the flash). */
    trace_init ();
    /* Get the color. */
    team_color = switch_get_color ();
    return FSM_NEXT (INIT_WAIT_FIRST_JACK_OUT, jack_removed_from_bot);
}

/*
 * do nothing.
 */
FSM_TRANS (INIT_WAIT_SECOND_JACK_IN,
	   jack_inserted_into_bot,
	   INIT_WAIT_FOR_HANDS_OUT)
{
    return FSM_NEXT (INIT_WAIT_SECOND_JACK_IN, jack_inserted_into_bot);
}

/*
 * fuck the wall in front.
 */
FSM_TRANS_TIMEOUT (INIT_WAIT_FOR_HANDS_OUT, 225,
		   INIT_GO_TO_THE_WALL)
{
    /* Go to the wall, no backward. */
    asserv_go_to_the_wall (0);
    return FSM_NEXT_TIMEOUT (INIT_WAIT_FOR_HANDS_OUT);
}

/*
 * reset the Y position of the bot.
 * reset the angular position of the bot.
 * move away from the wall (linear move).
 */
FSM_TRANS (INIT_GO_TO_THE_WALL,
	   bot_move_succeed,
	   INIT_GO_AWAY_FROM_THE_WALL)
{
    asserv_set_y_position (PG_Y (PG_LENGTH - BOT_SIZE_FRONT));
    asserv_set_angle_position (PG_A_DEG (90));
    /* Move away from the border. */
    asserv_move_linearly (- INIT_DIST);
    return FSM_NEXT (INIT_GO_TO_THE_WALL, bot_move_succeed);
}

/*
 * turn to face the other wall.
 */
FSM_TRANS (INIT_GO_AWAY_FROM_THE_WALL,
	   bot_move_succeed,
	   INIT_FACE_OTHER_WALL)
{
    /* Face the other wall. */
    asserv_goto_angle (PG_A_DEG (180));
    return FSM_NEXT (INIT_GO_AWAY_FROM_THE_WALL, bot_move_succeed);
}

/*
 * nothing to do.
 */
FSM_TRANS (INIT_FACE_OTHER_WALL,
	   bot_move_succeed,
	   INIT_WAIT_AFTER_ROTATION)
{
    return FSM_NEXT (INIT_FACE_OTHER_WALL, bot_move_succeed);
}

/*
 * fuck the wall in front.
 */
FSM_TRANS_TIMEOUT (INIT_WAIT_AFTER_ROTATION, 100,
		   INIT_GO_TO_THE_WALL_AGAIN)
{
    /* Go to the wall, no backward. */
    asserv_go_to_the_wall (0);
    return FSM_NEXT_TIMEOUT (INIT_WAIT_AFTER_ROTATION);
}

/*
 * reset the X position of the bot.
 * move away from the wall (linear move).
 */
FSM_TRANS (INIT_GO_TO_THE_WALL_AGAIN,
	   bot_move_succeed,
	   INIT_GO_AWAY_FROM_THE_WALL_AGAIN)
{
    asserv_set_x_position (PG_X (BOT_SIZE_FRONT));
    /* Move away from the border. */
    asserv_move_linearly (- INIT_DIST);
    return FSM_NEXT (INIT_GO_TO_THE_WALL_AGAIN, bot_move_succeed);
}

/*
 * go to the start position with a go to movement.
 */
FSM_TRANS (INIT_GO_AWAY_FROM_THE_WALL_AGAIN,
	   bot_move_succeed,
	   INIT_GO_TO_START_POSITION)
{
    /* Move away from the border. */
    asserv_goto_xya (BOT_INIT_XYA, 0);
    return FSM_NEXT (INIT_GO_AWAY_FROM_THE_WALL_AGAIN, bot_move_succeed);
}

/*
 * nothing to do, the bot is at the start position.
 */
FSM_TRANS (INIT_GO_TO_START_POSITION,
	   bot_move_succeed,
	   INIT_WAIT_SECOND_JACK_OUT)
{
    return FSM_NEXT (INIT_GO_TO_START_POSITION, bot_move_succeed);
}

/*
 * tell other FSM the match begins.
 * start the chrono.
 */
FSM_TRANS (INIT_WAIT_SECOND_JACK_OUT,
	   jack_removed_from_bot,
	   INIT_IDLE)
{
    /* Set the flag to transmit to other FSM. */
    init_match_is_started = 1;
    /* Start the chrono. */
    chrono_init ();
    return FSM_NEXT (INIT_WAIT_SECOND_JACK_OUT, jack_removed_from_bot);
}
