/* main.c */
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

#include "../../fsm.h"
#include "../../getsamples.h"
#include "../../playground.h"

#include "modules/utils/utils.h"

#include "io.h"

#include <stdio.h>

extern struct getsamples_data_t getsamples_data_;

int
main (void)
{
    /* Configure the get sample FSM */
    /* Go to our distributor */
    int16_t approach_angle = PG_DISTRIBUTOR_SAMPLE_OUR_A;
    uint8_t sample_bitfield = 0;
    /* We want to put the sample into the 0, 2 and 4 box */
    sample_bitfield |= _BV(0);
    sample_bitfield |= _BV(2);
    sample_bitfield |= _BV(4);

    /* Print initial state */
    /* Configure and initialize the get sample FSM */
    getsamples_start (approach_angle, sample_bitfield);
    /* Print first state */

    /* We are facing the distributor */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_bot_move_succeed);

    /* We have open the input hole */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_arm_move_succeed);

    /* The bot is now in contact with to the distributor */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_bot_move_succeed);

    /* Get the sample one by one */
    do
      {
	fsm_handle_event (&getsamples_fsm,
			  GETSAMPLES_EVENT_arm_pass_noted_position);
      } while (getsamples_data_.sample_bitfield);

    /* We need to do it one time again to make the bot move away from the
     * distributor */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_arm_pass_noted_position);

    /* We go away from the gutter */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_bot_move_succeed);

    /* We close the input hole */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_arm_move_succeed);

    return 0;
}
