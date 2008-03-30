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

#include "modules/proto/proto.h"
#include "modules/utils/utils.h"

#include "../../getsamples.h"
#include "../../getsamples_robo.h"

void
getsamples_print_test (getsamples_t *getsamples)
{
    printf ("STATE ");
    switch (getsamples->fsm)
      {
      case GETSAMPLES_STATE_START:
	printf ("START");
	break;
      case GETSAMPLES_STATE_PREPARE_ARM:
	printf ("PREPEARE_ARM");
	break;
      case GETSAMPLES_STATE_END:
	printf ("END");
	break;
      case GETSAMPLES_STATE_FORWARD_CONTROL:
	printf ("FORWARD_CONTROL");
	break;
      case GETSAMPLES_STATE_TAKE_SAMPLES:
	printf ("TAKE_SAMPLES");
	break;
      case GETSAMPLES_STATE_BACKWARD:
	printf ("BACKWARD");
	break;
      case GETSAMPLES_STATE_GO_TO_POSITION:
	printf ("GO_TO_POSITION");
	break;
      case GETSAMPLES_STATE_NB:
	printf ("STATE_NB");
      }
    printf ("\n");
}

int
main (void)
{
    getsamples_t getsamples_fsm;

    getsamples_init (&getsamples_fsm);
    getsamples_print_test (&getsamples_fsm);

    getsamples_fsm.distributor_x = 700;
    getsamples_fsm.distributor_y = 2100;
    getsamples_fsm.samples = 3;

    getsamples_handle_event (&getsamples_fsm,
			     GETSAMPLES_EVENT_ok);

    getsamples_print_test (&getsamples_fsm);

    getsamples_handle_event (&getsamples_fsm,
			     GETSAMPLES_EVENT_position_failed);

    getsamples_print_test (&getsamples_fsm);
    
    getsamples_handle_event (&getsamples_fsm,
			     GETSAMPLES_EVENT_position_reached);


    getsamples_print_test (&getsamples_fsm);

    getsamples_handle_event (&getsamples_fsm,
			     GETSAMPLES_EVENT_arm_moved);


    getsamples_print_test (&getsamples_fsm);

    getsamples_handle_event (&getsamples_fsm,
			 GETSAMPLES_EVENT_position_reached);

    getsamples_print_test (&getsamples_fsm);

    for (getsamples_fsm.samples--; getsamples_fsm.samples; getsamples_fsm.samples --)
      {
	getsamples_handle_event (&getsamples_fsm,
			 GETSAMPLES_EVENT_sample_took);

	getsamples_print_test (&getsamples_fsm);
      }

    getsamples_handle_event (&getsamples_fsm,
		     GETSAMPLES_EVENT_sample_took);

    getsamples_print_test (&getsamples_fsm);

    getsamples_handle_event (&getsamples_fsm,
		     GETSAMPLES_EVENT_position_reached);

    getsamples_print_test (&getsamples_fsm);

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
