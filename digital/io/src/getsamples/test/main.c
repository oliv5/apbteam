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
#include "../getsamples.h"
#include "../getsamples_robo.h"

#include <stdio.h>

void
getsamples_print_test (getsamples_t *getsamples)
{
    printf ("Machine state ");

    switch (getsamples->fsm)
      {
      case GETSAMPLES_STATE_PREPARE_ARM:
	printf ("PREPARE_ARM");
	break;
      case GETSAMPLES_STATE_END:
	printf ("END");
	break;
      case GETSAMPLES_STATE_FORWARD_CONTROL:
	printf ("FORWARD CONTROL");
	break;
      case GETSAMPLES_STATE_TAKE_SAMPLES:
	printf ("TAKE_SAMPLES");
	break;
      case GETSAMPLES_STATE_BACKWARD:
	printf ("BACKWARD");
	break;
      case GETSAMPLES_STATE_GO_TO_POSITION:
	printf ("GO TO POSITION");
	break;
      case GETSAMPLES_STATE_NB:
	printf ("NB");
      }
    printf ("\n");
}

int
main (void)
{
    getsamples_t getsamples_fsm;

    getsamples_init (&getsamples_fsm);
    getsamples_print_test (&getsamples_fsm);

    getsamples_handle_event (&getsamples_fsm,
			     GETSAMPLES_EVENT_position_failed);

    getsamples_print_test (&getsamples_fsm);
    
    getsamples_handle_event (&getsamples_fsm,
			     GETSAMPLES_EVENT_position_reached);


    getsamples_print_test (&getsamples_fsm);

    return 0;
}
