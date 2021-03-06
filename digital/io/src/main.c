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
#include "modules/devices/usdist/usdist.h"
#include "modules/devices/servo/servo_pos.h"
#include "modules/flash/flash.h"
#include "modules/trace/trace.h"
#include "events.h"

/* AVR include, non HOST */
#ifndef HOST
# include "switch.h"	/* Manage switches (jack, color selector) */
#else
#include <string.h>
#endif /* HOST */

#include "timer.h"
#include "simu.host.h"

#include "asserv.h"
#include "mimot.h"
#include "twi_master.h"
#include "eeprom.h"	/* Parameters loaded/stored in the EEPROM */
#define ANGFSM_NAME AI
#include "angfsm.h"
#include "fsm_queue.h"
#include "bot.h"
#include "radar.h"
#include "chrono.h"	/* chrono_end_match */
#include "pwm.h"
#include "playground_2010.h"
#include "contact.h"
#include "init.h"
#include "move.h"
#include "top.h"
#include "loader.h"

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
enum team_color_e team_color;

/** Obstacles positions, updated using radar module. */
vect_t main_obstacles_pos[2];

/** Number of obstacles in main_obstacles_pos. */
uint8_t main_obstacles_nb;

/**
 * US sensors stats counters.
 */
static uint8_t main_stats_usdist_, main_stats_usdist_cpt_;

/** Radar stats counters. */
static uint8_t main_stats_radar_, main_stats_radar_cpt_;

/**
 * Asserv stats counters.
 */
static uint8_t main_stats_asserv_, main_stats_asserv_cpt_;

/**
 * Main timer stats.
 */
static uint8_t main_stats_timer_;

/**
 * Main events management.
 * This function is responsible to get all events and send them to the
 * different FSM that want its.
 */
void
main_event_to_fsm (void)
{
    /* If an event is handled, stop generating any other event, because a
     * transition may have invalidated the current robot state. */
#define FSM_HANDLE_E(fsm, event) \
    do { if (FSM_HANDLE (fsm, event)) return; } while (0)
#define FSM_HANDLE_VAR_E(fsm, event) \
    do { if (FSM_HANDLE_VAR (fsm, event)) return; } while (0)
#define FSM_HANDLE_TIMEOUT_E(fsm) \
    do { if (FSM_HANDLE_TIMEOUT (fsm)) return; } while (0)
    /* Update FSM timeouts. */
    FSM_HANDLE_TIMEOUT_E (AI);

    /* If we have entering this function, last command of the asserv board has
     * been aquited. */
    FSM_HANDLE_E (AI, asserv_last_cmd_ack);

    asserv_status_e
	move_status = none,
	motor0_status = none,
	motor1_status = none,
	motorm0_status = none,
	motorm1_status = none;

    /* Get status of move, motor0 and motor1. */
    move_status = asserv_move_cmd_status ();
    motor0_status = asserv_motor0_cmd_status ();
    motor1_status = asserv_motor1_cmd_status ();
    motorm0_status = mimot_motor0_cmd_status ();
    motorm1_status = mimot_motor1_cmd_status ();

    /* Check commands move status. */
    if (move_status == success)
	FSM_HANDLE_E (AI, bot_move_succeed);
    else if (move_status == failure)
	FSM_HANDLE_E (AI, bot_move_failed);

    if (motor0_status == success)
	FSM_HANDLE_E (AI, elevator_succeed);
    else if (motor0_status == failure)
	FSM_HANDLE_E (AI, elevator_failed);

    if (motor1_status == success)
	FSM_HANDLE_E (AI, gate_succeed);
    else if (motor1_status == failure)
	FSM_HANDLE_E (AI, gate_failed);

    if (motorm0_status == success && motorm1_status == success)
	FSM_HANDLE_E (AI, clamp_succeed);
    else if (motorm0_status == failure || motorm1_status == failure)
	FSM_HANDLE_E (AI, clamp_failed);

    /* Check positions. */
    if (asserv_get_motor0_position () > BOT_ELEVATOR_UNLOAD_STEP)
	FSM_HANDLE_E (AI, elevator_unload_position);

    /* Contacts. */
    if (asserv_get_motor0_position () < BOT_ELEVATOR_REST_STEP
	&& (!IO_GET (CONTACT_BUMPER0) || !IO_GET (CONTACT_BUMPER1)))
	FSM_HANDLE_E (AI, loader_element);
    if (!IO_GET (CONTACT_STRATEGY))
      {
	if (switch_get_color ())
	    loader_down ();
	else
	    FSM_HANDLE_E (AI, hola_start);
      }

    /* Jack */
    if (switch_get_jack ())
	FSM_HANDLE_E (AI, jack_removed_from_bot);
    else
	FSM_HANDLE_E (AI, jack_inserted_into_bot);

    if (init_match_is_started)
      {
	FSM_HANDLE_E (AI, init_match_is_started);

	/* This must be done in the last part of this block. */
	init_match_is_started = 0;
      }

    /* Event generated at the end of the sub FSM to post to the top FSM */
    if (fsm_queue_poll ())
      {
	/* We must post the event at the end of this block because it
	 * will issue a continue and every instruction after will
	 * never be executed. */
	/* We need to save the event before reseting it */
	uint8_t save_event = fsm_queue_pop_event ();
	/* Post the event */
	FSM_HANDLE_VAR_E (AI, save_event);
      }

    /* Check obstacles. */
    if (move_check_obstacles ())
	return;

    /* Top events. */
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    if (robot_pos.v.y < PG_FIELD_Y_MAX)
	FSM_HANDLE_E (AI, in_field);

    /* TODO: Check other sensors */
}

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
    timer_init ();
    /* Load parameters */
    eeprom_load_param ();
    /* Dirty fix */
    utils_delay_ms (500);
    /* Asserv communication */
    asserv_init ();
    mimot_init ();
    /* TWI master. */
    twi_master_init ();
    /* Switch module */
    switch_init ();
    /* Path module */
    path_init (PG_BORDER_DISTANCE, PG_BORDER_DISTANCE,
	       PG_WIDTH - PG_BORDER_DISTANCE,
	       PG_LENGTH - PG_SLOPE_LENGTH - PG_BORDER_DISTANCE);
    /* Distance sensors. */
    usdist_init ();
    /* Top. */
    top_init ();
    /* Start FSM. */
    FSM_HANDLE (AI, start);
    /* PWM module */
    pwm_init ();
    /* Servo pos init. */
    servo_pos_init ();
    /* Contact. */
    contact_init ();

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
	  {
	    uint8_t timer_count = timer_wait ();
	    if (main_stats_timer_)
		proto_send1b('M', timer_count);
	    if (timer_count == 1)
	      {
		/* Main timer has reached overflow earlier!
		   We are late and this is really bad. */
		TRACE (TRACE_TIMER__LATE);
	      }
	  }

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

	/* Get the data from the UART */
	while (uart0_poll ())
	    /* Manage UART protocol */
	    proto_accept (uart0_getc ());

	/* Update PWM */
	pwm_update ();

	/* Update US distance sensors. */
	if (usdist_update ())
	  {
	    position_t robot_pos;
	    asserv_get_position (&robot_pos);
	    main_obstacles_nb = radar_update (&robot_pos, main_obstacles_pos);
	    move_obstacles_update ();
	    simu_send_pos_report (main_obstacles_pos, main_obstacles_nb, 0);
	  }

	/* Are TWI slaves synchronised? */
	if (twi_master_sync ())
	  {
	    /* First, update modules */
	    /* Update switch module */
	    switch_update ();
	    /* Update path module */
	    path_decay ();
	    /* Manage events. */
	    main_event_to_fsm ();
	  }

	/* Send stats if requested. */
	if (main_stats_usdist_ && !--main_stats_usdist_cpt_)
	  {
	    proto_send4w ('U', usdist_mm[0], usdist_mm[1], usdist_mm[2],
			  usdist_mm[3]);
	    main_stats_usdist_cpt_ = main_stats_usdist_;
	  }
	if (main_stats_radar_ && !--main_stats_radar_cpt_)
	  {
	    uint8_t blocking, i;
	    uint8_t buf[10];
	    position_t robot_pos;
	    asserv_get_position (&robot_pos);
	    vect_t step;
	    vect_from_polar_uf016 (&step, 1000, robot_pos.a);
	    vect_translate (&step, &robot_pos.v);
	    blocking = radar_blocking (&robot_pos.v, &step,
				       main_obstacles_pos, main_obstacles_nb);
	    buf[0] = blocking;
	    buf[1] = main_obstacles_nb;
	    for (i = 0; i < main_obstacles_nb; i++)
	      {
		buf[2 + 0 + i * 4] = v16_to_v8 (main_obstacles_pos[i].x, 1);
		buf[2 + 1 + i * 4] = v16_to_v8 (main_obstacles_pos[i].x, 0);
		buf[2 + 2 + i * 4] = v16_to_v8 (main_obstacles_pos[i].y, 1);
		buf[2 + 3 + i * 4] = v16_to_v8 (main_obstacles_pos[i].y, 0);
	      }
	    proto_send ('R', 2 + main_obstacles_nb * 4, buf);
	    main_stats_radar_cpt_ = main_stats_radar_;
	  }
	/* Send asserv stats if needed */
	if (main_stats_asserv_ && !--main_stats_asserv_cpt_)
	  {
	    /* Get current position */
	    position_t cur_pos;
	    asserv_get_position (&cur_pos);
	    /* Send stats */
	    proto_send3w ('A', cur_pos.v.x, cur_pos.v.y, cur_pos.a);
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
	servo_pos_set_high_time (args[0], &args[1]);
	break;

      case c ('P', 2):
	/* Set the high time values of a servo for the positions
	 *   - 1b: servo id number;
	 *   - 1b: servo position number;
	 */
	servo_pos_move_to (args[0], args[1]);
	break;


      case c ('s', 2):
	/* Set servo motor to a desired position using the servo module.
	 *   - 1b: servo id number;
	 *   - 1b: pwm high time value (position).
	 */
	servo_set_position (args[0], args[1]);
	break;

      case c ('S', 0):
	/* Report switch states. */
	proto_send1b ('S', switch_get_color () << 1 | switch_get_jack ());
	break;

      case c ('C', 0):
	/* Report contacts states. */
	proto_send1b ('C', PINC);
	break;

      case c ('M', 1):
	/* Main stats timer.
	 *   - 1b: 1 to enable, 0 to disable.
	 */
	main_stats_timer_ = args[0];
	break;

      case c ('w', 4):
	/* Set PWM.
	 *   - 1w: PWM value id number;
	 *   - 1w: pwm high time value (position).
	 */
	pwm_set (v8_to_v16 (args[0], args[1]), v8_to_v16 (args[2], args[3]));
	break;

      case c ('c', 1):
	/* Loader control.
	 * - 1b: command:
	 *   - 'd': move elevator down.
	 *   - 'u': move elevator up.
	 */
	switch (args[0])
	{
	case 'd':
	  loader_down ();
	  break;
	case 'u':
	  loader_up ();
	  break;
	}
	break;

      case c ('p', 8):
	/* Test path finding.
	 * - 1w: source x.
	 * - 1w: source y.
	 * - 1w: destination x.
	 * - 1w: destination y.
	 */
	  {
	    vect_t s, d;
	    s.x = v8_to_v16 (args[0], args[1]);
	    s.y = v8_to_v16 (args[2], args[3]);
	    d.x = v8_to_v16 (args[4], args[5]);
	    d.y = v8_to_v16 (args[6], args[7]);
	    path_endpoints (s, d);
	    path_update ();
	    vect_t p;
	    if (path_get_next (&p))
		proto_send0 ('p');
	  }
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
		  }
		break;
	      }
	    break;
	  }

	/* Stats commands.
	 * - b: interval between stats. */
      case c ('U', 1):
	/* US sensors stats. */
	main_stats_usdist_ = main_stats_usdist_cpt_ = args[0];
	break;
      case c ('R', 1):
	/* Radar stats. */
	main_stats_radar_ = main_stats_radar_cpt_ = args[0];
	break;
      case c ('A', 1):
	/* Position stats. */
	main_stats_asserv_ = main_stats_asserv_cpt_ = args[0];
	break;

	/* Asserv */
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
	      case 'S':
		/* Status. */
		  {
		    asserv_status_e move_status = asserv_move_cmd_status ();
		    asserv_status_e motor0_status = asserv_motor0_cmd_status ();
		    asserv_status_e motor1_status = asserv_motor1_cmd_status ();
		    asserv_status_e motorm0_status = mimot_motor0_cmd_status ();
		    asserv_status_e motorm1_status = mimot_motor1_cmd_status ();
		    proto_send2b ('S', move_status, (motor0_status << 0)
				  | (motor1_status << 2) | (motorm0_status << 4)
				  | (motorm1_status << 6));
		  }
		break;
	      }
	  }
	break;
      case c ('a', 2):
	  {
	    switch (args[0])
	      {
	      case 'f':
		/* Go to the wall */
		asserv_go_to_the_wall (args[1]);
		break;
	      case 'z':
		switch (args[1])
		  {
		  case 'e':
		    asserv_motor0_zero_position (-BOT_ELEVATOR_ZERO_SPEED);
		    break;
		  case 'a':
		    asserv_motor1_zero_position (0x10);
		    break;
		  }
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
		/* Move the motor0
		 *  - 2b: offset angle ;
		 *  - 1b: speed.
		 */
		asserv_move_motor0_absolute (v8_to_v16 (args[1], args[2]),
					     args[3]);
		break;
	      case 'c':
		/* Move the motor1
		 *  - 2b: position ;
		 *  - 1b: speed.
		 */
		asserv_move_motor1_absolute (v8_to_v16 (args[1], args[2]),
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
			     v8_to_v32 (args[5], args[6], args[7], args[8]),
			     0);
		break;
	      case 'r':
		/* Go to an absolute position (X,Y) in mm with backward
		 * allowed.
		 *  - 4b: x;
		 *  - 4b: y.
		 */
		asserv_goto (v8_to_v32 (args[1], args[2], args[3], args[4]),
			     v8_to_v32 (args[5], args[6], args[7], args[8]),
			     ASSERV_REVERT_OK);
		break;
	      }
	  }
	break;
      default:
	  {
	    uint8_t error = 1;
	    if (cmd == 'l')
		error = flash_log (size, args);
	    if (error)
	      {
		/* Unknown commands */
		proto_send0 ('?');
		return;
	      }
	  }
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
    /* produce AVR's FSM headers. */
    if (ANGFSM_OPTIONS (argc, argv))
        return 0;

    avr_init (argc, argv);

    /* Initialize the main and its subsystems */
    main_init ();

    /* Start the main loop */
    main_loop ();

    return 0;
}
