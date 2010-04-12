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
 * }}}
 * init:
 * place the bot in the start position for a match.
 */
#include "common.h"
#include "fsm.h"
#include "ai_cb.h"
#include "asserv.h"
#include "init.h"
#include "playground.h"
#include "main.h"
#include "bot.h"
#include "switch.h"
#include "chrono.h"
#include "playground.h"
#include "modules/trace/trace.h"
#include "modules/path/path.h"

/*
 * INIT_IDLE =start=>
 *  => INIT_WAIT_FIRST_JACK_IN
 *   do nothing.
 */
fsm_branch_t
ai__INIT_IDLE__start (void)
{
    return ai_next (INIT_IDLE, start);
}

/*
 * INIT_WAIT_FIRST_JACK_IN =jack_inserted_into_bot=>
 *  => INIT_WAIT_FIRST_JACK_OUT
 *   do nothing.
 */
fsm_branch_t
ai__INIT_WAIT_FIRST_JACK_IN__jack_inserted_into_bot (void)
{
    return ai_next (INIT_WAIT_FIRST_JACK_IN, jack_inserted_into_bot);
}

/*
 * INIT_WAIT_FIRST_JACK_OUT =jack_removed_from_bot=>
 *  => INIT_WAIT_SECOND_JACK_IN
 *   start trace module.
 *   get and store the color of the bot.
 */
fsm_branch_t
ai__INIT_WAIT_FIRST_JACK_OUT__jack_removed_from_bot (void)
{
    /* Initialize trace module (erase the flash). */
    trace_init ();
    /* Get the color. */
    bot_color = switch_get_color ();
    return ai_next (INIT_WAIT_FIRST_JACK_OUT, jack_removed_from_bot);
}

/*
 * INIT_WAIT_SECOND_JACK_IN =jack_inserted_into_bot=>
 *  => INIT_WAIT_FOR_HANDS_OUT
 *   do nothing.
 */
fsm_branch_t
ai__INIT_WAIT_SECOND_JACK_IN__jack_inserted_into_bot (void)
{
    return ai_next (INIT_WAIT_SECOND_JACK_IN, jack_inserted_into_bot);
}

/*
 * INIT_WAIT_FOR_HANDS_OUT =state_timeout=>
 *  => INIT_GO_TO_THE_WALL
 *   fuck the wall in front.
 */
fsm_branch_t
ai__INIT_WAIT_FOR_HANDS_OUT__state_timeout (void)
{
    /* Go to the wall, no backward. */
    asserv_go_to_the_wall (0);
    return ai_next (INIT_WAIT_FOR_HANDS_OUT, state_timeout);
}

/*
 * INIT_GO_TO_THE_WALL =bot_move_succeed=>
 *  => INIT_SET_Y_POSITION
 *   reset the Y position of the bot.
 */
fsm_branch_t
ai__INIT_GO_TO_THE_WALL__bot_move_succeed (void)
{
    asserv_set_y_position (PG_Y (PG_LENGTH - BOT_SIZE_FRONT));
    return ai_next (INIT_GO_TO_THE_WALL, bot_move_succeed);
}

/*
 * INIT_SET_Y_POSITION =asserv_last_cmd_ack=>
 *  => INIT_SET_ANGULAR_POSITION
 *   reset the angular position of the bot.
 */
fsm_branch_t
ai__INIT_SET_Y_POSITION__asserv_last_cmd_ack (void)
{
    /* We are facing top border. */
    asserv_set_angle_position (PG_A_DEG (90));
    return ai_next (INIT_SET_Y_POSITION, asserv_last_cmd_ack);
}

/*
 * INIT_SET_ANGULAR_POSITION =asserv_last_cmd_ack=>
 *  => INIT_GO_AWAY_FROM_THE_WALL
 *   move away from the wall (linear move).
 */
fsm_branch_t
ai__INIT_SET_ANGULAR_POSITION__asserv_last_cmd_ack (void)
{
    /* Move away from the border. */
    asserv_move_linearly (- INIT_DIST);
    return ai_next (INIT_SET_ANGULAR_POSITION, asserv_last_cmd_ack);
}

/*
 * INIT_GO_AWAY_FROM_THE_WALL =bot_move_succeed=>
 *  => INIT_FACE_OTHER_WALL
 *   turn to face the other wall.
 */
fsm_branch_t
ai__INIT_GO_AWAY_FROM_THE_WALL__bot_move_succeed (void)
{
    /* Face the other wall. */
    asserv_goto_angle (PG_A_DEG (180));
    return ai_next (INIT_GO_AWAY_FROM_THE_WALL, bot_move_succeed);
}

/*
 * INIT_FACE_OTHER_WALL =bot_move_succeed=>
 *  => INIT_WAIT_AFTER_ROTATION
 *   nothing to do.
 */
fsm_branch_t
ai__INIT_FACE_OTHER_WALL__bot_move_succeed (void)
{
    return ai_next (INIT_FACE_OTHER_WALL, bot_move_succeed);
}

/*
 * INIT_WAIT_AFTER_ROTATION =state_timeout=>
 *  => INIT_GO_TO_THE_WALL_AGAIN
 *   fuck the wall in front.
 */
fsm_branch_t
ai__INIT_WAIT_AFTER_ROTATION__state_timeout (void)
{
    /* Go to the wall, no backward. */
    asserv_go_to_the_wall (0);
    return ai_next (INIT_WAIT_AFTER_ROTATION, state_timeout);
}

/*
 * INIT_GO_TO_THE_WALL_AGAIN =bot_move_succeed=>
 *  => INIT_SET_X_POSITION
 *   reset the X position of the bot.
 */
fsm_branch_t
ai__INIT_GO_TO_THE_WALL_AGAIN__bot_move_succeed (void)
{
    asserv_set_x_position (PG_X (BOT_SIZE_FRONT));
    return ai_next (INIT_GO_TO_THE_WALL_AGAIN, bot_move_succeed);
}

/*
 * INIT_SET_X_POSITION =asserv_last_cmd_ack=>
 *  => INIT_GO_AWAY_FROM_THE_WALL_AGAIN
 *   move away from the wall (linear move).
 */
fsm_branch_t
ai__INIT_SET_X_POSITION__asserv_last_cmd_ack (void)
{
    /* Move away from the border. */
    asserv_move_linearly (- INIT_DIST);
    return ai_next (INIT_SET_X_POSITION, asserv_last_cmd_ack);
}

/*
 * INIT_GO_AWAY_FROM_THE_WALL_AGAIN =bot_move_succeed=>
 *  => INIT_GO_TO_START_POSITION
 *   go to the start position with a go to movement.
 */
fsm_branch_t
ai__INIT_GO_AWAY_FROM_THE_WALL_AGAIN__bot_move_succeed (void)
{
    /* Move away from the border. */
    asserv_goto_xya (PG_X (PG_START_ZONE_WIDTH / 2),
		     PG_Y (PG_LENGTH - PG_START_ZONE_LENGTH / 2),
		     PG_A_DEG (0), 0);
    return ai_next (INIT_GO_AWAY_FROM_THE_WALL_AGAIN, bot_move_succeed);
}

/*
 * INIT_GO_TO_START_POSITION =bot_move_succeed=>
 *  => INIT_WAIT_SECOND_JACK_OUT
 *   nothing to do, the bot is at the start position.
 */
fsm_branch_t
ai__INIT_GO_TO_START_POSITION__bot_move_succeed (void)
{
    return ai_next (INIT_GO_TO_START_POSITION, bot_move_succeed);
}

/*
 * INIT_WAIT_SECOND_JACK_OUT =jack_removed_from_bot=>
 *  => INIT_IDLE
 *   tell other FSM the match begins.
 *   start the chrono.
 */
fsm_branch_t
ai__INIT_WAIT_SECOND_JACK_OUT__jack_removed_from_bot (void)
{
    /* Set the flag to transmit to other FSM. */
    init_match_is_started = 1;
    /* Start the chrono. */
    chrono_init ();
    return ai_next (INIT_WAIT_SECOND_JACK_OUT, jack_removed_from_bot);
}


