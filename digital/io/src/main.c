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
#include "modules/path/path.h"

/* AVR include, non HOST */
#ifndef HOST
# include "switch.h"	/* Manage switches (jack, color selector) */
#endif /* HOST */

#include "main_timer.h"
#include "simu.host.h"

#include "asserv.h"	/* Functions to control the asserv board */
#include "eeprom.h"	/* Parameters loaded/stored in the EEPROM */
#include "fsm.h"	/* fsm_* */
#include "giboulee.h"	/* team_color */
/* #include "top.h" */	/* top_* */
#include "servo_pos.h"
#include "chrono.h"	/* chrono_end_match */
#include "sharp.h"	/* sharp module */
#include "pwm.h"
#include "playground.h"

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
 * Post a event to the top FSM in the next iteration of main loop.
 */
uint8_t main_post_event_for_top_fsm = 0xFF;

/**
 * Do not generate sharps event for FSM during a certain count of cycles.
 */
uint16_t main_sharp_ignore_event;

/**
 * Flag for homologation, to disable the path finding and always stop in front
 * of an obstacle and wait.
 * Do not touch anymore ! XXX FIXME TODO
 */
uint8_t main_always_stop_for_obstacle = 1;

/**
 * Post an event for the main loop to wake up the move FSM in a certain count
 * of cycles.
 */
uint16_t main_move_wait_cycle;

/**
 * Sharps stats counters.
 */
uint8_t main_stats_sharps, main_stats_sharps_cpt;
uint8_t main_stats_sharps_interpreted_, main_stats_sharps_interpreted_cpt_;

/**
 * Asserv stats counters.
 */
static uint8_t main_stats_asserv_, main_stats_asserv_cpt_;

/**
 * Main timer stats.
 */
static uint8_t main_stats_timer_;

/**
 * Update frequency of sharps.
 */
#define MAIN_SHARP_UPDATE_FREQ 5

/**
 * Sharps frequency counter.
 */
uint8_t main_sharp_freq_counter_;

/**
 * Initialize the main and all its subsystems.
 */
static void
main_init (void)
{
    /* Serial port */
    uart0_init ();
    /* Enable interrupts */
    sei ();
    /* Main timer */
    main_timer_init ();
    /* Load parameters */
    eeprom_load_param ();
    /* Dirty fix */
    utils_delay_ms (500);
    /* Asserv communication */
    asserv_init ();
    /* Switch module */
    switch_init ();
    /* Path module */
    path_init (PG_BORDER_DISTANCE, PG_BORDER_DISTANCE,
	       PG_WIDTH - PG_BORDER_DISTANCE, PG_HEIGHT - PG_BORDER_DISTANCE);
    /* Start the top FSM */
    //top_start ();
    fsm_init(&top_fsm);
    fsm_handle_event (&top_fsm, TOP_EVENT_start);
    /* Sharp module */
    sharp_init ();
    /* PWM module */
    pwm_init ();

    /* io initialization done */
    proto_send0 ('z');
}

/**
 * Main (and infinite) loop of the io program.
 */
static void
main_loop (void)
{
#define FSM_HANDLE_EVENT(fsm,event) \
      { if (fsm_handle_event (fsm,event)) \
	  { \
	    continue; \
	  } \
      }

    /* Infinite loop */
    while (1)
      {
	/* Wait for an overflow of the main timer (4.444ms) */
	  {
	    uint8_t timer_count = main_timer_wait ();
	    if (main_stats_timer_)
		proto_send1b('M', timer_count);
	  }

	/* Get the data from the UART */
	while (uart0_poll ())
	    /* Manage UART protocol */
	    proto_accept (uart0_getc ());

	/* Update chrono. */
	chrono_update ();
	/* Is match over? */
	if (chrono_is_match_over ())
	  {
	    /* End it and block here indefinitely */
	    chrono_end_match (42);
	    /* Safety */
	    return;
	  }

	/* Update PWM */
	pwm_update ();

	/* Update TWI module to get new data from the asserv board */
	asserv_update_status ();

	/* Is last command has been acknowledged? */
	if (asserv_last_cmd_ack () == 0)
	  {
	    /* Called function to manage retransmission */
	    asserv_retransmit ();
	  }
	else
	  {
	    /* First, update modules */
	    /* Update switch module */
	    switch_update ();
	    /* Update path module */
	    path_decay ();
	    /* Sharps module */
	    /* Update the ignore sharp event flag */
	    if (main_sharp_ignore_event)
		main_sharp_ignore_event--;
	    /* Update wait flag for move FSM */
	    if (main_move_wait_cycle)
		main_move_wait_cycle--;
	    /* Update sharp module if required and only every
	     * MAIN_SHARP_UPDATE_FREQ cycles */
	    if (++main_sharp_freq_counter_ == MAIN_SHARP_UPDATE_FREQ)
	      {
		/* Update sharps */
		sharp_update (0xff);
		/* Reset counter */
		main_sharp_freq_counter_ = 0;
	      }

	    /* Update main */
	    asserv_status_e move_status = asserv_last_cmd_ack ()
		? asserv_move_cmd_status () : none;

	    /* Check commands move status */
	    if (move_status == success)
	      {
		/* Pass it to all the FSM that need it */
		FSM_HANDLE_EVENT (&move_fsm,
				  MOVE_EVENT_bot_move_succeed);
	      }
	    else if (move_status == failure)
	      {
		/* Move failed */
		FSM_HANDLE_EVENT (&move_fsm,
				  MOVE_EVENT_bot_move_failed);
	      }
	    /* Jack */
	    FSM_HANDLE_EVENT (&top_fsm, switch_get_jack () ?
			      TOP_EVENT_jack_removed_from_bot :
			      TOP_EVENT_jack_inserted_into_bot);
	    /* Settings acknowledge */
	    /*
	    if (main_top_generate_settings_ack_event)
	      {
		FSM_HANDLE_EVENT (&top_fsm, TOP_EVENT_settings_acknowledged);
	      }
	      */
	    FSM_HANDLE_EVENT (&top_fsm, TOP_EVENT_settings_acknowledged);
	    /* Event generated at the end of the sub FSM to post to the top FSM */
	    if (main_post_event_for_top_fsm != 0xFF)
	      {
		/* We must post the event at the end of this block because it
		 * will issue a continue and every instruction after will
		 * never be executed. */
		/* We need to save the event before reseting it */
		uint8_t save_event = main_post_event_for_top_fsm;
		/* Reset */
		main_post_event_for_top_fsm = 0xFF;
		/* Post the event */
		FSM_HANDLE_EVENT (&top_fsm, save_event);
	      }
	    /* Sharps event for move FSM */
	    /* If we do not need to ignore sharp event */
	    if (!main_sharp_ignore_event)
	      {
		/* Get the current direction of the bot */
		uint8_t moving_direction = asserv_get_moving_direction ();
		/* If we are moving */
		if (moving_direction)
		  {
		    if (sharp_path_obstrued (moving_direction))
		      {
			/* Generate an event for move FSM */
			FSM_HANDLE_EVENT (&move_fsm,
					  MOVE_EVENT_bot_move_obstacle);
		      }
		  }
	      }
	    /* Wait flag for move FSM */
	    if (!main_move_wait_cycle)
	      {
		FSM_HANDLE_EVENT (&move_fsm, MOVE_EVENT_wait_finished);
	      }
	    /* TODO: Check other sensors */
	  }

	/* Send Sharps raw stats. */
	if (main_stats_sharps && !--main_stats_sharps_cpt)
	  {
	    uint8_t count;
	    uint8_t cache[SHARP_NUMBER * 2];
	    /* Reset counter */
	    main_stats_sharps_cpt = main_stats_sharps;
	    for (count = 0; count < SHARP_NUMBER; count++)
	      {
		uint16_t tmp = sharp_get_raw (count);
		cache[count * 2] = v16_to_v8 (tmp, 1);
		cache[count * 2 + 1] = v16_to_v8 (tmp, 0);
	      }
	    proto_send ('H', 2 * SHARP_NUMBER, cache);
	  }
	/* Send Sharps interpreted stats. */
	if (main_stats_sharps_interpreted_ &&
	    !--main_stats_sharps_interpreted_cpt_)
	  {
	    uint8_t count;
	    uint8_t cache[SHARP_NUMBER];
	    /* Reset counter */
	    main_stats_sharps_interpreted_cpt_ =
		main_stats_sharps_interpreted_;
	    for (count = 0; count < SHARP_NUMBER; count++)
	      {
		cache[count] = sharp_get_interpreted (count);
	      }
	    proto_send ('I', SHARP_NUMBER, cache);
	  }

	/* Send asserv stats if needed */
	if (main_stats_asserv_ && !--main_stats_asserv_cpt_)
	  {
	    /* Get current position */
	    asserv_position_t cur_pos;
	    asserv_get_position (&cur_pos);
	    /* Send stats */
	    proto_send3w ('A', cur_pos.x, cur_pos.y, cur_pos.a);
	    /* Reset stats counter */
	    main_stats_asserv_cpt_ = main_stats_asserv_;
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

	/* Servo pos commands */
      case c ('p', (SERVO_POS_NUMBER + 1)):
	/* Set the high time values of a servo for the positions
	 *   - 1b: servo id number;
	 *   - 1b: high time value for position 0;
	 *   - ...
	 */
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

      case c ('H', 1):
	/* Print raw stats for sharps.
	 *   - 1b: frequency of sharp stats.
	 */
	main_stats_sharps_cpt = main_stats_sharps = args[0];
	break;

      case c ('M', 1):
	/* Main stats timer.
	 *   - 1b: 1 to enable, 0 to disable.
	 */
	main_stats_timer_ = args[0];
	break;

      case c ('I', 1):
	/* Print interpreted stats for sharps.
	 *   - 1b: frequency of sharp stats.
	 */
	main_stats_sharps_interpreted_cpt_ = main_stats_sharps_interpreted_ =
	       args[0];
	break;

      case c ('h', 5):
	/* Configure sharps threshold.
	 *   - 1b: sharp id number;
	 *   - 2b: sharp low threshold;
	 *   - 2b: sharp high threshold.
	 */
	sharp_set_threshold (args[0], v8_to_v16 (args[1], args[2]),
			     v8_to_v16 (args[3], args[4]));
	break;

      case c ('w', 4):
	/* Set PWM.
	 *   - 1w: PWM value id number;
	 *   - 1w: pwm high time value (position).
	 */
	pwm_set (v8_to_v16 (args[0], args[1]), v8_to_v16 (args[2], args[3]));
	break;

	/* EEPROM command */
      case c ('e', 1):
	  {
	    /* Save/clear config
	     *  - 1b:
	     *    - 00, 'c': clear config
	     *    - 01, 's': save config
	     *    - 02, 'd': dump config
	     */
	    switch (args[0])
	      {
	      case 0:
	      case 'c':
		eeprom_clear_param ();
		break;
	      case 1:
	      case 's':
		eeprom_save_param ();
		break;
	      case 2:
	      case 'd':
		  {
		    uint8_t compt;
		    /* Trap */
		    for (compt = 0; compt < SERVO_NUMBER; compt++)
		      {
			proto_send ('p', SERVO_POS_NUMBER,
				    servo_pos_high_time[compt]);
		      }
		    /* Sharp */
		    for (compt = 0; compt < SHARP_NUMBER; compt++)
		      {
			proto_send5b
			    ('h', compt,
			     v16_to_v8 (sharp_threshold[compt][0], 1),
			     v16_to_v8 (sharp_threshold[compt][0], 0),
			     v16_to_v8 (sharp_threshold[compt][1], 1),
			     v16_to_v8 (sharp_threshold[compt][1], 0));
		      }
		  }
		break;
	      }
	    break;
	  }
	/* FSM commands */
      case c ('A', 1):
	  {
	    /* Get position stats
	     *   - 1b: frequency.
	     */
	    main_stats_asserv_ = main_stats_asserv_cpt_ = args[0];
	  }
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
	      case 'c':
		/* Move the elevator
		 *  - 2b: position ;
		 *  - 1b: speed.
		 */
		asserv_move_elevator_absolute (v8_to_v16 (args[1], args[2]),
					       args[3]);
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
      case c ('a', 9):
	  {
	    switch (args[0])
	      {
	      case 'x':
		/* Go to an absolute position (X,Y) in mm.
		 *  - 4b: x;
		 *  - 4b: y.
		 */
		asserv_goto (v8_to_v32 (args[1], args[2], args[3], args[4]),
			     v8_to_v32 (args[5], args[6], args[7], args[8]));
		break;
	      case 'r':
		/* Go to an absolute position (X,Y) in mm with backward
		 * allowed.
		 *  - 4b: x;
		 *  - 4b: y.
		 */
		asserv_goto_back (v8_to_v32 (args[1], args[2], args[3], args[4]),
			     v8_to_v32 (args[5], args[6], args[7], args[8]));
		break;
	      }
	  }
	break;
      case c ('o', 1):
	  {
	    /* Omo-logo-ation flag, to prevent avoiding obstacle and stop
	     * instead.
	     *   - 1b: state of the flag (0 to disable, 1 to enable).
	     * Do not touch anymore ! XXX FIXME TODO
	     */
	    main_always_stop_for_obstacle = args[0];
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
