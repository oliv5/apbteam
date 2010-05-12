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
#include "loader.h"

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
 *  => FIRST_GO_BEGIN_OF_LINE_FAST
 *   the match start
 *   set fast speed
 *   go to first sequence begin of line
 */
fsm_branch_t
ai__WAIT_INIT_TO_FINISH__init_match_is_started (void)
{
    asserv_set_speed (BOT_MOVE_FAST);
    position_t pos = PG_POSITION_DEG (375, 1503, -29);
    move_start (pos, 0);
    return ai_next (WAIT_INIT_TO_FINISH, init_match_is_started);
}

/*
 * FIRST_GO_BEGIN_OF_LINE_FAST =move_fsm_succeed=>
 *  => FIRST_GO_END_OF_LINE_FAST
 *   go to end of line
 *   loader down
 */
fsm_branch_t
ai__FIRST_GO_BEGIN_OF_LINE_FAST__move_fsm_succeed (void)
{
    position_t pos = PG_POSITION_DEG (2625, 253, -90);
    move_start (pos, 0);
    loader_down ();
    return ai_next (FIRST_GO_BEGIN_OF_LINE_FAST, move_fsm_succeed);
}

/*
 * FIRST_GO_BEGIN_OF_LINE_FAST =move_fsm_failed=>
 *  => FIRST_GO_BEGIN_OF_LINE_FAST
 *   set slow speed
 *   retry
 */
fsm_branch_t
ai__FIRST_GO_BEGIN_OF_LINE_FAST__move_fsm_failed (void)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    position_t pos = PG_POSITION_DEG (375, 1503, -29);
    move_start (pos, 0);
    return ai_next (FIRST_GO_BEGIN_OF_LINE_FAST, move_fsm_failed);
}

/*
 * FIRST_GO_END_OF_LINE_FAST =in_field=>
 *  => FIRST_GO_END_OF_LINE_SLOW
 *   set slow speed
 */
fsm_branch_t
ai__FIRST_GO_END_OF_LINE_FAST__in_field (void)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    return ai_next (FIRST_GO_END_OF_LINE_FAST, in_field);
}

/*
 * FIRST_GO_END_OF_LINE_FAST =move_fsm_succeed=>
 *  => IDLE
 *   set slow speed
 */
fsm_branch_t
ai__FIRST_GO_END_OF_LINE_FAST__move_fsm_succeed (void)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    ai__FIRST_GO_END_OF_LINE_SLOW__move_fsm_succeed ();
    return ai_next (FIRST_GO_END_OF_LINE_FAST, move_fsm_succeed);
}

/*
 * FIRST_GO_END_OF_LINE_FAST =move_fsm_failed=>
 *  => FIRST_GO_END_OF_LINE_SLOW
 *   set slow speed
 *   retry
 */
fsm_branch_t
ai__FIRST_GO_END_OF_LINE_FAST__move_fsm_failed (void)
{
    asserv_set_speed (BOT_MOVE_SLOW);
    ai__FIRST_GO_END_OF_LINE_SLOW__move_fsm_failed ();
    return ai_next (FIRST_GO_END_OF_LINE_FAST, move_fsm_failed);
}

/*
 * FIRST_GO_END_OF_LINE_SLOW =move_fsm_succeed=>
 *  => IDLE
 */
fsm_branch_t
ai__FIRST_GO_END_OF_LINE_SLOW__move_fsm_succeed (void)
{
    return ai_next (FIRST_GO_END_OF_LINE_SLOW, move_fsm_succeed);
}

/*
 * FIRST_GO_END_OF_LINE_SLOW =move_fsm_failed=>
 *  => FIRST_GO_END_OF_LINE_SLOW
 *   retry
 */
fsm_branch_t
ai__FIRST_GO_END_OF_LINE_SLOW__move_fsm_failed (void)
{
    position_t pos = PG_POSITION_DEG (2625, 253, -90);
    move_start (pos, 0);
    return ai_next (FIRST_GO_END_OF_LINE_SLOW, move_fsm_failed);
}

