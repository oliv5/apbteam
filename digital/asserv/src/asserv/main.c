/* main.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
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
#include "modules/utils/byte.h"
#include "modules/math/fixed/fixed.h"
#include "io.h"

#include "misc.h"
#include "state.h"

#include "counter.h"
#include "pwm.h"
#include "pos.h"
#include "speed.h"
#include "postrack.h"
#include "traj.h"
#include "aux.h"

#include "twi_proto.h"
#include "eeprom.h"

#ifndef HOST
# include "timer.h"
#else
# include "simu.host.h"
#endif

/** Report command completion. */
uint8_t main_sequence_ack = 4, main_sequence_ack_cpt;

/** Report of counters. */
uint8_t main_stat_counter, main_stat_counter_cpt;

/** Report of position. */
uint8_t main_stat_postrack, main_stat_postrack_cpt;

/** Report of auxiliary position. */
uint8_t main_stat_aux_pos, main_stat_aux_pos_cpt;

/** Statistics about speed control. */
uint8_t main_stat_speed, main_stat_speed_cpt;

/** Statistics about shaft position control. */
uint8_t main_stat_pos, main_stat_pos_cpt;

/** Statistics about pwm values. */
uint8_t main_stat_pwm, main_stat_pwm_cpt;

/** Report of timer. */
uint8_t main_stat_timer, main_stat_timer_cpt;

/** Print input port. */
uint8_t main_print_pin, main_print_pin_cpt;

#ifdef HOST
/** Simulation data. */
uint8_t main_simu, main_simu_cpt;
#endif /* HOST */

/** Record timer value at different stage of computing. Used for performance
 * analisys. */
uint8_t main_timer[6];

static void
main_loop (void);

/** Entry point. */
int
main (int argc, char **argv)
{
    avr_init (argc, argv);
    /* Pull-ups. */
    PORTC = 0xff;
    PORTD = 0xf0;
    PORTE = 0xff;
    PORTF = 0xfc;
    PORTG = 0x18;
    LED_SETUP;
    pwm_init ();
    timer_init ();
    counter_init ();
    uart0_init ();
    twi_proto_init ();
    postrack_init ();
    eeprom_read_params ();
    proto_send0 ('z');
    sei ();
    while (1)
	main_loop ();
    return 0;
}

/** Main loop. */
static void
main_loop (void)
{
    main_timer[5] = timer_read ();
    timer_wait ();
    /* Counter update. */
    counter_update ();
    main_timer[0] = timer_read ();
    /* Position control. */
    pos_update ();
    main_timer[1] = timer_read ();
    /* Pwm setup. */
    pwm_update ();
    main_timer[2] = timer_read ();
    /* Compute absolute position. */
    postrack_update ();
    aux_pos_update ();
    /* Compute trajectory. */
    if (state_main.mode >= MODE_TRAJ)
	traj_update ();
    if (state_aux0.mode >= MODE_TRAJ)
	aux_traj_update ();
    /* Speed control. */
    speed_update ();
    main_timer[3] = timer_read ();
    /* Stats. */
    if (main_sequence_ack
	&& (state_main.sequence_ack != state_main.sequence_finish
	    || state_aux0.sequence_ack != state_aux0.sequence_finish)
	&& !--main_sequence_ack_cpt)
      {
	proto_send2b ('A', state_main.sequence_finish,
		      state_aux0.sequence_finish);
	main_sequence_ack_cpt = main_sequence_ack;
      }
    if (main_stat_counter && !--main_stat_counter_cpt)
      {
	proto_send3w ('C', counter_left, counter_right, counter_aux0);
	main_stat_counter_cpt = main_stat_counter;
      }
    if (main_stat_postrack && !--main_stat_postrack_cpt)
      {
	proto_send3d ('X', postrack_x, postrack_y, postrack_a);
	main_stat_postrack_cpt = main_stat_postrack;
      }
    if (main_stat_aux_pos && !--main_stat_aux_pos_cpt)
      {
	proto_send1w ('Y', aux0.pos);
	main_stat_aux_pos_cpt = main_stat_aux_pos;
      }
    if (main_stat_speed && !--main_stat_speed_cpt)
      {
	proto_send3b ('S', speed_theta.cur >> 8, speed_alpha.cur >> 8,
		      speed_aux0.cur >> 8);
	main_stat_speed_cpt = main_stat_speed;
      }
    if (main_stat_pos && !--main_stat_pos_cpt)
      {
	proto_send6w ('P', pos_theta.e_old, pos_theta.i,
		      pos_alpha.e_old, pos_alpha.i,
		      pos_aux0.e_old, pos_aux0.i);
	main_stat_pos_cpt = main_stat_pos;
      }
#ifdef HOST
    if (main_simu && !--main_simu_cpt)
      {
	proto_send3w ('H', (uint16_t) simu_pos_x,
		      (uint16_t) simu_pos_y,
		      (uint16_t) (simu_pos_a * 1024));
	main_simu_cpt = main_simu;
      }
#endif /* HOST */
    if (main_stat_pwm && !--main_stat_pwm_cpt)
      {
	proto_send3w ('W', pwm_left, pwm_right, pwm_aux0);
	main_stat_pwm_cpt = main_stat_pwm;
      }
    if (main_stat_timer && !--main_stat_timer_cpt)
      {
	proto_send6b ('T', main_timer[0], main_timer[2], main_timer[3],
		      main_timer[4], main_timer[4], main_timer[5]);
	main_stat_timer_cpt = main_stat_timer;
      }
    if (main_print_pin && !--main_print_pin_cpt)
      {
	proto_send1b ('I', PINC);
	main_print_pin_cpt = main_print_pin;
      }
    /* Misc. */
    while (uart0_poll ())
	proto_accept (uart0_getc ());
    twi_proto_update ();
    main_timer[4] = timer_read ();
}

/** Handle incoming messages. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset. */
	utils_reset ();
	break;
    /* Commands. */
      case c ('w', 0):
	/* Set zero pwm. */
	pos_reset (&pos_theta);
	pos_reset (&pos_alpha);
	state_main.mode = MODE_PWM;
	pwm_left = 0;
	pwm_right = 0;
	break;
      case c ('w', 4):
	/* Set pwm.
	 * - w: left pwm.
	 * - w: right pwm. */
	pos_reset (&pos_theta);
	pos_reset (&pos_alpha);
	state_main.mode = MODE_PWM;
	PWM_SET (pwm_left, v8_to_v16 (args[0], args[1]));
	PWM_SET (pwm_right, v8_to_v16 (args[2], args[3]));
	break;
      case c ('w', 2):
	/* Set auxiliary pwm.
	 * - w: pwm. */
	pos_reset (&pos_aux0);
	state_aux0.mode = MODE_PWM;
	PWM_SET (pwm_aux0, v8_to_v16 (args[0], args[1]));
	break;
      case c ('c', 4):
	/* Add to position consign.
	 * - w: theta consign offset.
	 * - w: alpha consign offset. */
	state_main.mode = MODE_POS;
	state_main.variant = 0;
	pos_theta.cons += v8_to_v16 (args[0], args[1]);
	pos_alpha.cons += v8_to_v16 (args[2], args[3]);
	break;
      case c ('c', 2):
	/* Add to auxiliary position consign.
	 * - w: consign offset. */
	state_aux0.mode = MODE_POS;
	state_aux0.variant = 0;
	pos_aux0.cons += v8_to_v16 (args[0], args[1]);
	break;
      case c ('s', 0):
	/* Stop (set zero speed). */
	state_main.mode = MODE_SPEED;
	state_main.variant = 0;
	speed_theta.use_pos = speed_alpha.use_pos = 0;
	speed_theta.cons = 0;
	speed_alpha.cons = 0;
	break;
      case c ('s', 2):
	/* Set speed.
	 * - b: theta speed.
	 * - b: alpha speed. */
	state_main.mode = MODE_SPEED;
	state_main.variant = 0;
	speed_theta.use_pos = speed_alpha.use_pos = 0;
	speed_theta.cons = args[0] << 8;
	speed_alpha.cons = args[1] << 8;
	break;
      case c ('s', 1):
	/* Set auxiliary speed.
	 * - b: speed. */
	state_aux0.mode = MODE_SPEED;
	state_aux0.variant = 0;
	speed_aux0.use_pos = 0;
	speed_aux0.cons = args[0] << 8;
	break;
      case c ('s', 9):
	/* Set speed controlled position consign.
	 * - d: theta consign offset.
	 * - d: alpha consign offset.
	 * - b: sequence number. */
	if (args[8] == state_main.sequence)
	    break;
	speed_theta.use_pos = speed_alpha.use_pos = 1;
	speed_theta.pos_cons = pos_theta.cons;
	speed_theta.pos_cons += v8_to_v32 (args[0], args[1], args[2], args[3]);
	speed_alpha.pos_cons = pos_alpha.cons;
	speed_alpha.pos_cons += v8_to_v32 (args[4], args[5], args[6], args[7]);
	state_start (&state_main, MODE_SPEED, args[8]);
	break;
      case c ('s', 5):
	/* Set auxiliary speed controlled position consign.
	 * - d: consign offset.
	 * - b: sequence number. */
	if (args[4] == state_aux0.sequence)
	    break;
	speed_aux0.use_pos = 1;
	speed_aux0.pos_cons = pos_aux0.cons;
	speed_aux0.pos_cons += v8_to_v32 (args[0], args[1], args[2], args[3]);
	state_start (&state_aux0, MODE_SPEED, args[4]);
	break;
      case c ('l', 5):
	/* Set linear speed controlled position consign.
	 * - d: consign offset.
	 * - b: sequence number. */
	if (args[4] == state_main.sequence)
	    break;
	speed_theta.use_pos = speed_alpha.use_pos = 1;
	speed_theta.pos_cons = pos_theta.cons;
	speed_theta.pos_cons += v8_to_v32 (args[0], args[1], args[2], args[3]);
	speed_alpha.pos_cons = pos_alpha.cons;
	state_start (&state_main, MODE_SPEED, args[4]);
	break;
      case c ('a', 5):
	/* Set angular speed controlled position consign.
	 * - d: angle offset.
	 * - b: sequence number. */
	if (args[4] == state_main.sequence)
	    break;
	traj_angle_offset_start (v8_to_v32 (args[0], args[1], args[2],
					    args[3]), args[4]);
	break;
      case c ('f', 1):
	/* Go to the wall.
	 * - b: sequence number. */
	if (args[0] == state_main.sequence)
	    break;
	traj_ftw_start (args[0]);
	break;
      case c ('F', 1):
	/* Go to the dispenser.
	 * - b: sequence number. */
	if (args[0] == state_main.sequence)
	    break;
	traj_gtd_start (args[0]);
	break;
      case c ('x', 9):
	/* Go to position.
	 * - d: x, f24.8.
	 * - d: y, f24.8.
	 * - b: sequence number. */
	if (args[8] == state_main.sequence)
	    break;
	traj_goto_start (v8_to_v32 (args[0], args[1], args[2], args[3]),
			 v8_to_v32 (args[4], args[5], args[6], args[7]),
			 0, args[8]);
	break;
      case c ('r', 9):
	/* Go to position, backward allowed.
	 * - d: x, f24.8.
	 * - d: y, f24.8.
	 * - b: sequence number. */
	if (args[8] == state_main.sequence)
	    break;
	traj_goto_start (v8_to_v32 (args[0], args[1], args[2], args[3]),
			 v8_to_v32 (args[4], args[5], args[6], args[7]),
			 1, args[8]);
	break;
      case c ('x', 3):
	/* Go to angle.
	 * - d: a, f0.16.
	 * - b: sequence number. */
	if (args[2] == state_main.sequence)
	    break;
	traj_goto_angle_start (v8_to_v32 (0, args[0], args[1], 0),
			       args[2]);
	break;
      case c ('x', 11):
	/* Go to position, then angle.
	 * - d: x, f24.8.
	 * - d: y, f24.8.
	 * - w: a, f0.16.
	 * - b: sequence number. */
	if (args[10] == state_main.sequence)
	    break;
	traj_goto_xya_start (v8_to_v32 (args[0], args[1], args[2], args[3]),
			     v8_to_v32 (args[4], args[5], args[6], args[7]),
			     v8_to_v32 (0, args[8], args[9], 0),
			     0, args[10]);
	break;
      case c ('r', 11):
	/* Go to position, then angle, backward allowed.
	 * - d: x, f24.8.
	 * - d: y, f24.8.
	 * - w: a, f0.16.
	 * - b: sequence number. */
	if (args[10] == state_main.sequence)
	    break;
	traj_goto_xya_start (v8_to_v32 (args[0], args[1], args[2], args[3]),
			     v8_to_v32 (args[4], args[5], args[6], args[7]),
			     v8_to_v32 (0, args[8], args[9], 0),
			     1, args[10]);
	break;
      case c ('y', 3):
	/* Auxiliary go to position.
	 * - w: pos, i16.
	 * - b: sequence number. */
	if (args[2] == state_aux0.sequence)
	    break;
	aux_traj_goto_start (v8_to_v16 (args[0], args[1]), args[2]);
	break;
      case c ('y', 1):
	/* Auxiliary find zero.
	 * - b: sequence number. */
	if (args[0] == state_aux0.sequence)
	    break;
	aux_traj_find_zero_start (args[0]);
	break;
      case c ('a', 2):
	/* Set both acknoledge.
	 * - b: main ack sequence number.
	 * - b: auxiliary ack sequence number. */
	state_acknowledge (&state_aux0, args[1]);
	/* no break; */
      case c ('a', 1):
	/* Set main acknoledge.
	 * - b: main ack sequence number. */
	state_acknowledge (&state_main, args[0]);
	break;
    /* Stats.
     * - b: interval between stats. */
      case c ('A', 1):
	/* Command completion report. */
	main_sequence_ack_cpt = main_sequence_ack = args[0];
	break;
      case c ('C', 1):
	/* Counter stats. */
	main_stat_counter_cpt = main_stat_counter = args[0];
	break;
      case c ('X', 1):
	/* Position stats. */
	main_stat_postrack_cpt = main_stat_postrack = args[0];
	break;
      case c ('Y', 1):
	/* Auxiliary position stats. */
	main_stat_aux_pos_cpt = main_stat_aux_pos = args[0];
	break;
      case c ('S', 1):
	/* Motor speed control stats. */
	main_stat_speed_cpt = main_stat_speed = args[0];
	break;
      case c ('P', 1):
	/* Motor position control stats. */
	main_stat_pos_cpt = main_stat_pos = args[0];
	break;
      case c ('W', 1):
	/* Pwm stats. */
	main_stat_pwm_cpt = main_stat_pwm = args[0];
	break;
      case c ('T', 1):
	/* Timing stats. */
	main_stat_timer_cpt = main_stat_timer = args[0];
	break;
      case c ('I', 1):
	/* Input port stats. */
	main_print_pin_cpt = main_print_pin = args[0];
	break;
#ifdef HOST
      case c ('H', 1):
	/* Simulation data. */
	main_simu_cpt = main_simu = args[0];
	break;
#endif /* HOST */
      default:
	/* Params. */
	if (cmd == 'p')
	  {
	    switch (c (args[0], size))
	      {
	      case c ('X', 1):
		/* Reset position. */
		postrack_x = 0;
		postrack_y = 0;
		postrack_a = 0;
		break;
	      case c ('X', 5):
		/* Set current x position.
		 * - d: x position. */
		postrack_x = v8_to_v32 (args[1], args[2], args[3], args[4]);
		break;
	      case c ('Y', 5):
		/* Set current y position.
		 * - d: y position. */
		postrack_y = v8_to_v32 (args[1], args[2], args[3], args[4]);
		break;
	      case c ('A', 5):
		/* Set current angle.
		 * - d: angle. */
		postrack_a = v8_to_v32 (args[1], args[2], args[3], args[4]);
		break;
	      case c ('c', 5):
		/* Set right counter correction factor.
		 * - d: factor (f8.24). */
		counter_right_correction = v8_to_v32 (args[1], args[2],
						      args[3], args[4]);
		break;
	      case c ('f', 3):
		/* Set footing.
		 * - w: footing. */
		postrack_set_footing (v8_to_v16 (args[1], args[2]));
		break;
	      case c ('a', 5):
		/* Set main acceleration.
		 * - w: theta.
		 * - w: alpha. */
		speed_theta.acc = v8_to_v16 (args[1], args[2]);
		speed_alpha.acc = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('a', 3):
		/* Set auxiliary acceleration.
		 * - w: acceleration. */
		speed_aux0.acc = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('s', 5):
		/* Set main maximum and slow speed.
		 * - b: theta max.
		 * - b: alpha max.
		 * - b: theta slow.
		 * - b: alpha slow. */
		speed_theta.max = args[1];
		speed_alpha.max = args[2];
		speed_theta.slow = args[3];
		speed_alpha.slow = args[4];
		break;
	      case c ('s', 3):
		/* Set auxiliary maximum and slow speed.
		 * - b: speed max.
		 * - b: speed slow. */
		speed_aux0.max = args[1];
		speed_aux0.slow = args[2];
		break;
	      case c ('p', 3):
		pos_aux0.kp = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('p', 5):
		pos_theta.kp = v8_to_v16 (args[1], args[2]);
		pos_alpha.kp = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('i', 3):
		pos_aux0.ki = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('i', 5):
		pos_theta.ki = v8_to_v16 (args[1], args[2]);
		pos_alpha.ki = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('d', 3):
		pos_aux0.kd = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('d', 5):
		pos_theta.kd = v8_to_v16 (args[1], args[2]);
		pos_alpha.kd = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('E', 3):
		pos_e_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('I', 3):
		pos_i_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('D', 3):
		pos_d_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('b', 3):
		pos_blocked = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('e', 5):
		traj_eps = v8_to_v16 (args[1], args[2]);
		traj_aeps = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('w', 2):
		/* Set PWM direction.
		 * - b: bits: 0000[aux0][right][left]. */
		pwm_set_reverse (args[1]);
		break;
	      case c ('E', 2):
		/* Write to eeprom.
		 * - b: 00: clear config, 01: write config. */
		if (args[1])
		    eeprom_write_params ();
		else
		    eeprom_clear_params ();
		break;
	      case c ('P', 1):
		/* Print current settings. */
		proto_send1b ('E', EEPROM_KEY);
		proto_send1d ('c', counter_right_correction);
		proto_send1w ('f', postrack_footing);
		proto_send2w ('a', speed_theta.acc, speed_alpha.acc);
		proto_send4b ('s', speed_theta.max, speed_alpha.max,
			      speed_theta.slow, speed_alpha.slow);
		proto_send2w ('p', pos_theta.kp, pos_alpha.kp);
		proto_send2w ('i', pos_theta.ki, pos_alpha.ki);
		proto_send2w ('d', pos_theta.kd, pos_alpha.kd);
		proto_send1w ('a', speed_aux0.acc);
		proto_send2b ('s', speed_aux0.max, speed_aux0.slow);
		proto_send1w ('p', pos_aux0.kp);
		proto_send1w ('i', pos_aux0.ki);
		proto_send1w ('d', pos_aux0.kd);
		proto_send1w ('E', pos_e_sat);
		proto_send1w ('I', pos_i_sat);
		proto_send1w ('D', pos_d_sat);
		proto_send1w ('b', pos_blocked);
		proto_send1w ('e', traj_eps);
		proto_send1b ('w', pwm_reverse);
		break;
	      default:
		proto_send0 ('?');
		return;
	      }
	  }
#ifdef HOST
	else if (cmd == 'h')
	  {
	    switch (c (args[0], size))
	      {
	      case c ('X', 7):
		/* Set simulated position.
		 * - w: x position.
		 * - w: y position.
		 * - w: angle (rad) * 1024. */
		simu_pos_x = (double) v8_to_v16 (args[1], args[2]);
		simu_pos_y = (double) v8_to_v16 (args[3], args[4]);
		simu_pos_a = (double) v8_to_v16 (args[5], args[6]) / 1024;
		break;
	      }
	  }
#endif /* HOST */
	else
	  {
	    proto_send0 ('?');
	    return;
	  }
	break;
      }
    proto_send (cmd, size, args);
#undef c
}

