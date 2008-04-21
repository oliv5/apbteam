/* main.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Dufour Jérémy
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
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"
#include "modules/utils/utils.h"

/* AVR include, non HOST */
#ifndef HOST
# include "main_timer.avr.h"
# include "switch.h"	/* Manage switches (jack, color selector) */
#endif /* HOST */

#include "simu.host.h"

#include "asserv.h"	/* Functions to control the asserv board */
#include "eeprom.h"	/* Parameters loaded/stored in the EEPROM */
#include "trap.h"	/* Trap module (trap_* functions) */
#include "fsm.h"	/* fsm_* */
#include "giboulee.h"	/* team_color */
#include "getsamples.h"	/* getsamples_start */

#include "io.h"

/**
 * Initialize the main and all its subsystems.
 */
static void main_init (void);

/**
 * Main (and infinite) loop of the io program.
 */
static void main_loop (void);

/**
 * Our color.
 */
enum team_color_e bot_color;

/**
 * Initialize the main and all its subsystems.
 */
static void
main_init (void)
{
    /* Serial port */
    uart0_init ();
    /* Main timer */
    main_timer_init ();
    /* Load parameters */
    eeprom_load_param ();
    /* Asserv communication */
    asserv_init ();
    /* Trap module */
    trap_init ();
    /* Switch module */
    switch_init ();

    /* Enable interrupts */
    sei ();

    /* io initialization done */
    proto_send0 ('z');
}

/**
 * Main (and infinite) loop of the io program.
 */
static void
main_loop (void)
{
    /* Infinite loop */
    while (1)
      {
	/* Wait for an overflow of the main timer (4.444ms) */
	main_timer_wait ();

	/* Get the data from the UART */
	while (uart0_poll ())
	    /* Manage UART protocol */
	    proto_accept (uart0_getc ());

	/* Update TWI module to get new data from the asserv board */
	asserv_update_status ();

	/* Update switch module */
	switch_update ();

	/* Is last command has been acknowledged? */
	if (asserv_last_cmd_ack () == 0)
	    /* Called function to manage retransmission */
	    asserv_retransmit ();
	else
	  {
	    /* Check commands move status */
	    if (asserv_move_cmd_status () == success)
	      {
		/* Pass it to all the FSM that need it */
		fsm_handle_event (&getsamples_fsm,
				  GETSAMPLES_EVENT_bot_move_succeed);
	      }
	    else
	      {
		/* Move failed */
		fsm_handle_event (&getsamples_fsm,
				  GETSAMPLES_EVENT_bot_move_failed);
	      }
	    /* Check commands arm status */
	    if (asserv_arm_cmd_status () == success)
	      {
		/* Pass it to all the FSM that need it */
		fsm_handle_event (&getsamples_fsm,
				  GETSAMPLES_EVENT_arm_move_succeed);
	      }
	    /* TODO: Check if the sensor placed at the noted position has seen
	     * an arm passed and forward this event to the getsamples FSM */
	    if (asserv_arm_position_reached ())
	      {
		/* Reset the notifier */
		asserv_arm_set_position_reached (0);
		fsm_handle_event (&getsamples_fsm,
				  GETSAMPLES_EVENT_arm_pass_noted_position);
	      }
	    /* Jack */
	    fsm_handle_event (&top_fsm, switch_get_jack () ?
			      TOP_EVENT_jack_removed_from_bot :
			      TOP_EVENT_jack_inserted_into_bot);
	    /* TODO: Check other sensors */
	  }
      }
}

/**
 * Manage received UART commands.
 * @param cmd commands received.
 * @param size the length of arguments.
 * @param args the argument of the command.
 */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset */
	utils_reset ();
	break;

	/* Servo/trap commands */
      case c ('t', 3):
	/* Set the high time values of a servo for the vertical and horizontal
	 * positions using trapdoor module.
	 *   - 1b: servo id number;
	 *   - 1b: high time value (horizontal);
	 *   - 1b: high time value (vertical).
	 */
	trap_set_high_time (args[0], args[1], args[2]);
	break;

      case c ('T', 1):
	/* Setup traps to open a path to a destination box.
	 *   - 1b: box identification
	 */
	trap_setup_path_to_box (args[0]);
	break;

      case c ('s', 2):
	/* Set servo motor to a desired position using the servo module.
	 *   - 1b: servo id number;
	 *   - 1b: pwm high time value (position).
	 */
	servo_set_high_time (args[0], args[1]);
	break;

      case c ('S', 0):
	/* Report switch states. */
	proto_send1b ('S', switch_get_color () << 1 | switch_get_jack ());
	break;

	/* EEPROM command */
      case c ('e', 1):
	/* Save/clear config
	 *  - 1b:
	 *    - 00: clear config
	 *    - other values: save config
	 */
	if (args[0] == 0)
	    eeprom_clear_param ();
	else
	    eeprom_save_param ();
	break;

	/* FSM commands */
      case c ('g', 2):
	/* Start the get samples FSM
	 *   - 1b: the approach angle to face the distributor ;
	 *   - 1b: how many and where to put collected samples ;
	 */
	getsamples_start (args[0] << 8, args[1]);
	break;

	/* Asserv/arm */
      case c ('a', 1):
	  {
	    switch (args[0])
	      {
	      case 'w':
		/* Free motor */
		asserv_free_motor ();
		break;
	      case 's':
		/* Stop motor */
		asserv_stop_motor ();
		break;
	      case 'f':
		/* Go to the wall */
		asserv_go_to_the_wall ();
		break;
	      case 'F':
		/* Go to the distributor */
		asserv_go_to_distributor ();
		break;
	      case 'c':
		/* Close the input hole with the arm */
		asserv_close_input_hole ();
		break;
	      }
	  }
	break;
      case c ('a', 3):
	  {
	    switch (args[0])
	      {
	      case 'y':
		/* Angular move
		 *  - 2b: angle of rotation
		 */
		asserv_goto_angle (v8_to_v16 (args[1], args[2]));
		break;
	      }
	  }
	break;
      case c ('a', 4):
	  {
	    switch (args[0])
	      {
	      case 'b':
		/* Move the arm
		 *  - 2b: offset angle ;
		 *  - 1b: speed.
		 */
		asserv_move_arm (v8_to_v16 (args[1], args[2]), args[3]);
		break;
	      }
	  }
	break;
      case c ('a', 5):
	  {
	    switch (args[0])
	      {
	      case 'l':
		/* Linear move
		 *  - 4b: distance to move.
		 */
		asserv_move_linearly (v8_to_v32 (args[1], args[2], args[3],
						 args[4]));
		break;
	      }
	  }
	break;

      default:
	/* Unknown commands */
	proto_send0 ('?');
	return;
      }
    /* When no error, acknowledge commands */
    proto_send (cmd, size, args);
#undef c
}

/**
 * Main function.
 */
int
main (int argc, char **argv)
{
    avr_init (argc, argv);

    /* Initialize the main and its subsystems */
    main_init ();

    /* Start the main loop */
    main_loop ();

    return 0;
}
