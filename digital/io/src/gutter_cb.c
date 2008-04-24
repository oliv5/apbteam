/* gutter_cb.c - gutter FSM callbacks. */
/*  {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include "gutter_cb.h"

#include "asserv.h"	/* asserv_go_to_the_wall */
#include "trap.h"	/* trap_* */
#include "playground.h"	/* PG_GUTTER_A */
#include "main.h"	/* main_post_event_for_top_fsm */

/**
 * Gutter private data to wait a certain number of cycles.
 */
uint16_t gutter_wait_cycle_;

/**
 * Count of cycles to wait before we estimate all the balls have been dropped
 * into the gutter. A cycle normally last 4.4ms.
 */
#define GUTTER_WAIT_FOR_BALLS_TO_DROP 1126

/*
 * ROTATE_REAR_SIDE_TO_GUTTER =bot_move_succeed=>
 *  => GO_TO_THE_GUTTER_WALL
 *   make the bot reversing against the gutter
 */
fsm_branch_t
gutter__ROTATE_REAR_SIDE_TO_GUTTER__bot_move_succeed (void)
{
    /* Make the bot reversing against the gutter */
    asserv_go_to_the_wall ();
    return gutter_next (ROTATE_REAR_SIDE_TO_GUTTER, bot_move_succeed);
}

/*
 * IDLE =start=>
 *  => ROTATE_REAR_SIDE_TO_GUTTER
 *   put the bot back to the gutter
 */
fsm_branch_t
gutter__IDLE__start (void)
{
    /* Initialize internal data */
    gutter_wait_cycle_ = 0;
    /* Put the bot back to the gutter */
    asserv_goto_angle (PG_GUTTER_A);
    return gutter_next (IDLE, start);
}

/*
 * GO_TO_THE_GUTTER_WALL =bot_move_succeed=>
 *  => DROP_BALLS
 *   open the collector to drop the balls
 *   wait for a while
 */
fsm_branch_t
gutter__GO_TO_THE_GUTTER_WALL__bot_move_succeed (void)
{
    /* Open the rear panel */
    trap_open_rear_panel ();
    /* Wait for GUTTER_WAIT_FOR_BALLS_TO_DROP before being calling back by the
     * main loop */
    gutter_wait_cycle_ = GUTTER_WAIT_FOR_BALLS_TO_DROP;
    return gutter_next (GO_TO_THE_GUTTER_WALL, bot_move_succeed);
}

/*
 * DROP_BALLS =wait_finished=>
 *  => IDLE
 *   close the rear panel
 */
fsm_branch_t
gutter__DROP_BALLS__wait_finished (void)
{
    /* Close the rear panel */
    trap_close_rear_panel ();
    /* Tell the top FSM we have finished */
    main_post_event_for_top_fsm = TOP_EVENT_gutter_fsm_finished + 1;
    return gutter_next (DROP_BALLS, wait_finished);
}
