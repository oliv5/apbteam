/* getsamples.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
#include "getsamples.h"
#include "fsm.h"
#include "trap.h"

struct getsamples_data_t getsamples_data;

/** Start a getsamples FSM. */
void
getsamples_start (uint32_t distributor_x, uint32_t distributor_y,
		  uint8_t samples, uint8_t event_to_post)
{
    /* Set parameters. */
    getsamples_data.distributor_x = distributor_x;
    getsamples_data.distributor_y = distributor_y;
    getsamples_data.samples = samples;
    getsamples_data.event = event_to_post;
    /* Start FSM. */
    fsm_init (&getsamples_fsm);
    fsm_handle_event (&getsamples_fsm, GETSAMPLES_EVENT_ok);
}

/** Configure the classifier using the bit fields in the getsamples_data
 * structure.
 */
void
getsamples_configure_classifier (void)
{
    switch (getsamples_data.samples)
      {
      case 0x15:
	trap_setup_path_to_box (out_left_box);
	getsamples_data.samples &= 0xF;
	break;
      case 0xA:
	trap_setup_path_to_box (middle_left_box);
	getsamples_data.samples &= 0x7;
	break;
      case 0x5:
	trap_setup_path_to_box (middle_box);
	getsamples_data.samples &= 0x3;
	break;
      case 0x2:
	trap_setup_path_to_box (middle_right_box);
	getsamples_data.samples &= 0x1;
	break;
      case 0x1:
	trap_setup_path_to_box (out_right_box);
	getsamples_data.samples = 0x0;
	break;
      default:
	trap_setup_path_to_box (garbage);
      }
}
