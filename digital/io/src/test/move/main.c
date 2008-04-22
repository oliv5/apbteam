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
#include "../../asserv.h"
#include "../../move.h"

#include <stdio.h>

/**
 * Status structure maintains by the update command.
 */
typedef struct asserv_struct_s
{
    /** Status flags. */
    uint8_t status;
    /** Sequence number. */
    uint8_t seq;
    /** Bot position. */
    asserv_position_t position;
    /** Arm position. */
    uint16_t arm_position;
} asserv_struct_s;

/**
 * Status variable.
 */
asserv_struct_s asserv_status;


void
move_print_test (fsm_t *move)
{
    printf ("Machine state ");

    switch (move->state_current)
      {
	case MOVE_STATE_IDLE:
	  printf ("IDLE");
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
    move_data.position_x = asserv_status.position.x = 1500;
    move_data.position_y = asserv_status.position.y = 1050;

    fsm_init (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_start);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_blocked);
    move_print_test (&move_fsm);
    
    fsm_handle_event (&move_fsm, MOVE_EVENT_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_reached);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_reached);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_blocked);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_reached);
    move_print_test (&move_fsm);

    fsm_handle_event (&move_fsm, MOVE_EVENT_reached);
    move_print_test (&move_fsm);

    return 0;
}

void
asserv_goto (uint32_t x, uint32_t y)
{
    printf ("x : %d\n", x);
    printf ("y : %d\n", y);
}

void
asserv_get_position (asserv_position_t *pos)
{
    pos->x = asserv_status.position.x;
    pos->y = asserv_status.position.y;
}

