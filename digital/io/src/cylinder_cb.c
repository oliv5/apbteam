/* cylinder_cb.c - cylinder FSM callbacks. */
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
#include "cylinder_cb.h"

/*
 * WAIT_FOR_BRIDGE_READY =bridge_ready=>
 *  => TURN_PLUS_2_AGAIN
 *   bridge clear, delivering puck...
 */
fsm_branch_t
cylinder__WAIT_FOR_BRIDGE_READY__bridge_ready (void)
{
    return cylinder_next (WAIT_FOR_BRIDGE_READY, bridge_ready);
}

/*
 * WAIT_A_PUCK =new_puck=>
 * bridge_ready => TURN_PLUS_4
 *   put puck to the bridge
 * bridge_full => TURN_PLUS_2
 *   bridge full, waiting before release puck to the bridge
 */
fsm_branch_t
cylinder__WAIT_A_PUCK__new_puck (void)
{
    return cylinder_next_branch (WAIT_A_PUCK, new_puck, bridge_ready);
    return cylinder_next_branch (WAIT_A_PUCK, new_puck, bridge_full);
}

/*
 * INIT =init_done=>
 *  => WAIT_A_PUCK
 *   the cylinder is ready to get pucks
 */
fsm_branch_t
cylinder__INIT__init_done (void)
{
    return cylinder_next (INIT, init_done);
}

/*
 * WAIT_FOR_PUCKS_RELEASE =lift_ready=>
 *  => TURN_PLUS_1
 *   robot ready for new puck, open door
 */
fsm_branch_t
cylinder__WAIT_FOR_PUCKS_RELEASE__lift_ready (void)
{
    return cylinder_next (WAIT_FOR_PUCKS_RELEASE, lift_ready);
}

/*
 * IDLE =jack_in=>
 *  => INIT
 *   initialize robot
 */
fsm_branch_t
cylinder__IDLE__jack_in (void)
{
    return cylinder_next (IDLE, jack_in);
}

/*
 * TURN_PLUS_2 =move_done=>
 *  => WAIT_FOR_BRIDGE_READY
 *   bridge full, waiting for Bison Fute's clearance
 */
fsm_branch_t
cylinder__TURN_PLUS_2__move_done (void)
{
    return cylinder_next (TURN_PLUS_2, move_done);
}

/*
 * PROBE_OF =probe_done=>
 * puck_detected_and_lift_full => TURN_MINUS_3
 *   BAD CASE, there is an undesirable puck to the cylinder
 * puck_detected_and_lift_ready => WAIT_FOR_BRIDGE_READY
 *   there is a puck, we take a shortcut...
 * cylinder_empty_and_lift_ready => WAIT_A_PUCK
 *   ready for a new puck
 * cylinder_empty_and_lift_full => TURN_MINUS_1
 *   stop accepting new puck
 */
fsm_branch_t
cylinder__PROBE_OF__probe_done (void)
{
    return cylinder_next_branch (PROBE_OF, probe_done, puck_detected_and_lift_full);
    return cylinder_next_branch (PROBE_OF, probe_done, puck_detected_and_lift_ready);
    return cylinder_next_branch (PROBE_OF, probe_done, cylinder_empty_and_lift_ready);
    return cylinder_next_branch (PROBE_OF, probe_done, cylinder_empty_and_lift_full);
}

/*
 * TURN_PLUS_1 =move_done=>
 *  => WAIT_A_PUCK
 *   door opened, ready for a new puck
 */
fsm_branch_t
cylinder__TURN_PLUS_1__move_done (void)
{
    return cylinder_next (TURN_PLUS_1, move_done);
}

/*
 * TURN_MINUS_1 =move_done=>
 *  => WAIT_FOR_PUCKS_RELEASE
 *   robot entrance closed
 */
fsm_branch_t
cylinder__TURN_MINUS_1__move_done (void)
{
    return cylinder_next (TURN_MINUS_1, move_done);
}

/*
 * TURN_MINUS_3 =move_done=>
 *  => WAIT_FOR_PUCKS_RELEASE
 *   evil puck dropped and robot entrance closed
 */
fsm_branch_t
cylinder__TURN_MINUS_3__move_done (void)
{
    return cylinder_next (TURN_MINUS_3, move_done);
}

/*
 * TURN_PLUS_2_AGAIN =move_done=>
 *  => PROBE_OF
 *   there is a puck to the cylinder?
 */
fsm_branch_t
cylinder__TURN_PLUS_2_AGAIN__move_done (void)
{
    return cylinder_next (TURN_PLUS_2_AGAIN, move_done);
}

/*
 * TURN_PLUS_4 =move_done=>
 *  => PROBE_OF
 *   there is a puck to the cylinder?
 */
fsm_branch_t
cylinder__TURN_PLUS_4__move_done (void)
{
    return cylinder_next (TURN_PLUS_4, move_done);
}


