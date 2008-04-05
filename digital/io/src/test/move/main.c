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
move_print_test (fsm_t *move)
{
    printf ("Machine state ");

    switch (move->state_current)
      {
	case MOVE_STATE_END:
	  printf ("END");
	  break;
	case MOVE_STATE_DESIRED_POSITION:
	  printf ("DESIRED POSITION");
	  break;
	case MOVE_STATE_MOVE_ON_RIGHT:
	  printf ("MOVE ON RIGHT");
	  break;
	case MOVE_STATE_MOVE_ON_LEFT:
	  printf ("MOVE ON LEFT");
	  break;
	default:
	  printf ("STATE_NB");
      }
    printf ("\n");
}

int
main (void)
{
    fsm_init (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_ok);

    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_failed_or_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_failed_or_blocked);
    move_print_test (&move_fsm);
    
    fsm_handle_event (&move_fsm, MOVE_EVENT_near_border);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_failed_or_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_reached);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_failed_or_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_reached);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_reached);
    move_print_test (&move_fsm);

    return 0;
}

//void
//asserv_set_x_position (uint32_t position)
//{
//    printf ("X position : %d\n", position);
//}
//
//void
//asserv_set_y_position (int32_t y)
//{
//    printf ("Y position : %d\n", y);
//}
