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

/**
 * Print current state of the FSM get sample.
 */
void
getsamples_print_test (fsm_t *getsamples)
{
    printf ("STATE ");
    switch (getsamples->state_current)
      {
      case GETSAMPLES_STATE_IDLE:
	printf ("IDLE");
	break;
      case GETSAMPLES_STATE_FACE_DISTRIBUTOR:
	printf ("FACE_DISTRIBUTOR");
	break;
      case GETSAMPLES_STATE_OPEN_INPUT_HOLE:
	printf ("OPEN_INPUT_HOLE");
	break;
      case GETSAMPLES_STATE_APPROACH_DISTRIBUTOR:
	printf ("APPROACH_DISTRIBUTOR");
	break;
      case GETSAMPLES_STATE_TAKE_SAMPLES:
	printf ("TAKE_SAMPLES");
	break;
      case GETSAMPLES_STATE_MOVE_AWAY_FROM_DISTRIBUTOR:
	printf ("MOVE_AWAY_FROM_DISTRIBUTOR");
	break;
      case GETSAMPLES_STATE_CLOSE_INPUT_HOLE:
	printf ("CLOSE_INPUT_HOLE");
	break;
      default:
	printf ("Unhandled case?!");
      }
    printf ("\n");
}

/* Yerk export */
enum team_color_e bot_color = RED_TEAM;

extern uint8_t sample_bitfield_;

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
    getsamples_print_test (&getsamples_fsm);
    /* Configure and initialize the get sample FSM */
    getsamples_start (approach_angle, sample_bitfield);
    /* Print first state */
    getsamples_print_test (&getsamples_fsm);

    /* We are facing the distributor */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_bot_move_succeed);
    getsamples_print_test (&getsamples_fsm);

    /* We have open the input hole */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_arm_move_succeed);
    getsamples_print_test (&getsamples_fsm);

    /* The bot is now in contact with to the distributor */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_bot_move_succeed);
    getsamples_print_test (&getsamples_fsm);

    /* Get the sample one by one */
    do
      {
	fsm_handle_event (&getsamples_fsm,
			  GETSAMPLES_EVENT_arm_pass_noted_position);
	getsamples_print_test (&getsamples_fsm);
      } while (sample_bitfield_);

    /* We need to do it one time again to make the bot move away from the
     * distributor */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_arm_pass_noted_position);
    getsamples_print_test (&getsamples_fsm);

    /* We go away from the gutter */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_bot_move_succeed);
    getsamples_print_test (&getsamples_fsm);

    /* We close the input hole */
    fsm_handle_event (&getsamples_fsm,
		      GETSAMPLES_EVENT_arm_move_succeed);
    getsamples_print_test (&getsamples_fsm);

    return 0;
}


static uint16_t asserv_arm_position = 0;

/* Define functions for debug */
void
trap_setup_path_to_box (uint8_t box_id)
{
    printf ("[trap] Configure trap doors to open %d.\n", box_id);
}

void
asserv_move_linearly (int32_t distance)
{
    printf ("[asserv] Make the bot move linearly of %d mm.\n", distance);
}

void
asserv_move_arm (uint16_t position, uint8_t speed)
{
    asserv_arm_position += position;
    printf ("[asserv] Move arm at %d (speed: %d).\n",
	    asserv_arm_position, speed);
}

void
asserv_close_input_hole (void)
{
    printf ("[asserv] Put the arm in front of the input hole.\n");
    asserv_move_arm (asserv_arm_position %
		     BOT_ARM_THIRD_ROUND, BOT_ARM_SPEED);
}

uint16_t
asserv_get_arm_position (void)
{
    return asserv_arm_position;
}

void
asserv_arm_set_position_reached (uint16_t position)
{
    printf ("[asserv] Arm notifier at position %d (we are at %d).\n",
	    position, asserv_arm_position);
}

void
asserv_go_to_distributor (void)
{
    printf ("[asserv] Go to distributor.\n");
}

void
asserv_get_position (void *undef)
{
    printf ("[asserv] Asking position of the bot.\n");
}

void
asserv_goto (uint32_t x, uint32_t y)
{
    printf ("[asserv] Move the bot to (%d; %d).\n", x, y);
}

void
asserv_goto_angle (int16_t angle)
{
    printf ("[asserv] Move the bot to face %X.\n", angle);
}

void
asserv_set_x_position (int32_t x)
{
    printf ("[asserv] Set X position to %d.\n", x);
}

void
asserv_set_y_position (int32_t y)
{
    printf ("[asserv] Set Y position to %d.\n", y);
}

void
asserv_set_angle_position (int16_t a)
{
    printf ("[asserv] Set angle position to %d.\n", a);
}

void
gutter_start (void)
{
    printf ("[FSM:gutter] Start the gutter FSM\n");
}
