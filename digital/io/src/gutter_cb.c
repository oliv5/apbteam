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
#include "gutter.h"
#include "gutter_cb.h"
#include "trap.h"
#include "modules/utils/utils.h"

/*
 * IDLE =start=>
 *  => OPEN_COLLECTOR
 *   Open the collector and wait for a while.
 */
fsm_branch_t
gutter__IDLE__start (void)
{
    // Open the rear panel.
    trap_open_rear_panel ();
    return gutter_next (IDLE, start);
}


/*
 * CLOSE_COLLECTOR =collector_closed=>
 *  => END
 *   The samples has been inserted in the gutter.
 */
fsm_branch_t
gutter__CLOSE_COLLECTOR__collector_closed (void)
{
    //Close the collector.
    trap_close_rear_panel();
    return gutter_next (CLOSE_COLLECTOR, collector_closed);
}

/*
 * OPEN_COLLECTOR =collector_opened=>
 *  => CLOSE_COLLECTOR
 *   Wait some time and clse the door.
 */
fsm_branch_t
gutter__OPEN_COLLECTOR__collector_opened (void)
{
    utils_delay_ms(4.4);
    return gutter_next (OPEN_COLLECTOR, collector_opened);
}

