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

#include <stdio.h>

void
gutter_print_test (fsm_t *gutter)
{
    printf ("Machine state ");

    switch (gutter->state_current)
      {
	case GUTTER_STATE_END:
	  printf ("END");
	  break;
	case GUTTER_STATE_CLOSE_COLLECTOR:
	  printf ("CLOSE COLLECTOR");
	  break;
	case GUTTER_STATE_GO_TO_GUTTER:
	  printf ("GO_TO_GUTTER");
	  break;
	case GUTTER_STATE_OPEN_COLLECTOR:
	  printf ("OPEN COLLECTOR");
	  break;
	default:
	  printf ("STATE_NB");
      }
    printf ("\n");
}

int
main (void)
{
    fsm_init (&gutter_fsm);

    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_ok);

    gutter_print_test (&gutter_fsm);

    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_position_failed);

    gutter_print_test (&gutter_fsm);
    
    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_position_reached);

    gutter_print_test (&gutter_fsm);

    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_collector_opened);

    gutter_print_test (&gutter_fsm);

    fsm_handle_event (&gutter_fsm, GUTTER_EVENT_collector_closed);

    gutter_print_test (&gutter_fsm);

    return 0;
}

void
asserv_set_x_position (uint32_t position)
{
    printf ("X position : %d\n", position);
}

void
asserv_move_arm (uint16_t position, uint8_t speed)
{
    printf ("Move arm, position : %d, speed : %d\n", position, speed);
}

void
asserv_set_y_position (int32_t y)
{
    printf ("Y position : %d\n", y);
}

void
trap_open_rear_panel (void)
{
    printf ("Open rear panel\n");
}

void
trap_close_rear_panel(void)
{
    printf ("Close rear panel\n");
}

void
asserv_go_to_the_wall (void)
{
    printf ("Go to gutter\n");
}
