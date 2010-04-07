/* ai_top_cb.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
 * Main FSM calling other FSM.
 */
#include "common.h"
#include "fsm.h"
#include "ai_cb.h"
#include "top.h"
#include "move.h"
#include "chrono.h"
#include "playground.h"
#include "asserv.h"

/**
 * Time limit.
 */
#define TOP_TIME_LIMIT 25000

/*
 * IDLE =start=>
 *  => WAIT_INIT_TO_FINISH
 *   nothing to do.
 */
fsm_branch_t
ai__IDLE__start (void)
{
    return ai_next (IDLE, start);
}

/*
 * WAIT_INIT_TO_FINISH =init_match_is_started=>
 *  => GO_FAR
 *   the match start
 *   go to a far point
 */
fsm_branch_t
ai__WAIT_INIT_TO_FINISH__init_match_is_started (void)
{
    asserv_position_t pos;
    pos.x = PG_X_VALUE_COMPUTING (2000);
    pos.y = 1000;
    pos.a = PG_A_VALUE_COMPUTING (0);
    move_start (pos, 0);
    return ai_next (WAIT_INIT_TO_FINISH, init_match_is_started);
}

/*
 * GO_FAR =move_fsm_succeed=>
 *  => GO_NEAR
 *   go to a near point
 */
fsm_branch_t
ai__GO_FAR__move_fsm_succeed (void)
{
    asserv_position_t pos;
    pos.x = PG_X_VALUE_COMPUTING (1000);
    pos.y = 1000;
    pos.a = PG_A_VALUE_COMPUTING (0);
    move_start (pos, 0);
    return ai_next (GO_FAR, move_fsm_succeed);
}

/*
 * GO_NEAR =move_fsm_succeed=>
 *  => GO_FAR
 *   restart
 */
fsm_branch_t
ai__GO_NEAR__move_fsm_succeed (void)
{
    asserv_position_t pos;
    pos.x = PG_X_VALUE_COMPUTING (2000);
    pos.y = 1000;
    pos.a = PG_A_VALUE_COMPUTING (0);
    move_start (pos, 0);
    return ai_next (GO_NEAR, move_fsm_succeed);
}
