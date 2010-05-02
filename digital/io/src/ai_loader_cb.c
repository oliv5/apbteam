/* ai_loader_cb.c */
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
 * }}} */
#include "common.h"
#include "fsm.h"
#include "ai_cb.h"
#include "asserv.h"
#include "bot.h"

/*
 * LOADER_IDLE =start=>
 *  => LOADER_WAIT_JACK_IN
 */
fsm_branch_t
ai__LOADER_IDLE__start (void)
{
    return ai_next (LOADER_IDLE, start);
}

/*
 * LOADER_WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => LOADER_WAIT_JACK_OUT
 */
fsm_branch_t
ai__LOADER_WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    return ai_next (LOADER_WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * LOADER_WAIT_JACK_OUT =jack_removed_from_bot=>
 *  => LOADER_INIT_ELEVATOR_ZERO
 *   find elevator zero
 */
fsm_branch_t
ai__LOADER_WAIT_JACK_OUT__jack_removed_from_bot (void)
{
    asserv_motor0_zero_position (-BOT_ELEVATOR_ZERO_SPEED);
    return ai_next (LOADER_WAIT_JACK_OUT, jack_removed_from_bot);
}

/*
 * LOADER_INIT_ELEVATOR_ZERO =elevator_succeed=>
 *  => LOADER_INIT_ELEVATOR_UP
 *   move elevator up
 */
fsm_branch_t
ai__LOADER_INIT_ELEVATOR_ZERO__elevator_succeed (void)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_STROKE_STEP,
				 BOT_ELEVATOR_SPEED);
    return ai_next (LOADER_INIT_ELEVATOR_ZERO, elevator_succeed);
}

/*
 * LOADER_INIT_ELEVATOR_UP =elevator_succeed=>
 *  => LOADER_IDLE
 */
fsm_branch_t
ai__LOADER_INIT_ELEVATOR_UP__elevator_succeed (void)
{
    return ai_next (LOADER_INIT_ELEVATOR_UP, elevator_succeed);
}

/*
 * LOADER_INIT_ELEVATOR_UP =elevator_failed=>
 *  => LOADER_IDLE
 *   initialisation failure
 */
fsm_branch_t
ai__LOADER_INIT_ELEVATOR_UP__elevator_failed (void)
{
    /* Move so that the operator notice it. */
    asserv_move_motor0_absolute (BOT_ELEVATOR_STROKE_STEP / 3,
				 BOT_ELEVATOR_ZERO_SPEED);
    return ai_next (LOADER_INIT_ELEVATOR_UP, elevator_failed);
}

