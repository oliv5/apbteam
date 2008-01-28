/* main.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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

/** Motor command sequence, do not use values above 127, do not use zero. */
uint8_t main_sequence, main_sequence_ack, main_sequence_finish;

/* This is implementation include. */
#ifndef HOST
# include "timer.avr.c"
# include "counter.avr.c"
# include "pwm.avr.c"
#else
# include "simu.host.h"
#endif
#include "pos.c"
#include "speed.c"
#include "postrack.c"
#include "traj.c"
#ifndef HOST
# include "eeprom.avr.c"
#endif

/** Motor control mode:
 * 0: pwm setup.
 * 1: shaft position control.
 * 2: speed control.
 * 3: trajectory control. */
int8_t main_mode;

/** Report trajectory end. */
uint8_t main_sequence_ack_cpt = 2;

/** Report of counters. */
uint8_t main_stat_counter, main_stat_counter_cpt;

/** Report of position. */
uint8_t main_stat_postrack, main_stat_postrack_cpt;

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

/* +AutoDec */

/** Main loop. */
static void
main_loop (void);

/* -AutoDec */

/** Entry point. */
int
main (int argc, char **argv)
{
    avr_init (argc, argv);
    DDRD = 0x60;
    /* Pull-ups. */
    PORTA = 0xff;
    PORTC = 0xff;
    eeprom_read_params ();
    pwm_init ();
    timer_init ();
    counter_init ();
    uart0_init ();
    postrack_init ();
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
    /* Postion control. */
    if (main_mode >= 1)
	pos_update ();
    main_timer[1] = timer_read ();
    /* Pwm setup. */
    pwm_update ();
    main_timer[2] = timer_read ();
    /* Compute absolute position. */
    postrack_update ();
    /* Compute trajectory. */
    if (main_mode >= 3)
	traj_update ();
    /* Speed control. */
    if (main_mode >= 2)
	speed_update ();
    main_timer[3] = timer_read ();
    /* Stats. */
    if (main_sequence_ack != main_sequence_finish && !--main_sequence_ack_cpt)
      {
	proto_send1b ('A', main_sequence_finish);
	main_sequence_ack_cpt = 4;
      }
    if (main_stat_counter && !--main_stat_counter_cpt)
      {
	proto_send2w ('C', counter_left, counter_right);
	main_stat_counter_cpt = main_stat_counter;
      }
    if (main_stat_postrack && !--main_stat_postrack_cpt)
      {
	proto_send3d ('X', postrack_x, postrack_y, postrack_a);
	main_stat_postrack_cpt = main_stat_postrack;
      }
    if (main_stat_speed && !--main_stat_speed_cpt)
      {
	proto_send2b ('S', speed_theta_cur >> 8, speed_alpha_cur >> 8);
	main_stat_speed_cpt = main_stat_speed;
      }
    if (main_stat_pos && !--main_stat_pos_cpt)
      {
	proto_send4w ('P', pos_theta_e_old, pos_theta_int,
		      pos_alpha_e_old, pos_alpha_int);
	main_stat_pos_cpt = main_stat_pos;
      }
#ifdef HOST
    if (main_simu && !--main_simu_cpt)
      {
	proto_send3w ('Y', (uint16_t) simu_pos_x,
		      (uint16_t) simu_pos_y,
		      (uint16_t) (simu_pos_a * 1024));
	proto_send3b ('Z', traj_mode, 0, 0);
	main_simu_cpt = main_simu;
      }
#endif /* HOST */
    if (main_stat_pwm && !--main_stat_pwm_cpt)
      {
	proto_send2w ('W', pwm_left, pwm_right);
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
	proto_send2b ('I', PINA, PINF & 0xf);
	main_print_pin_cpt = main_print_pin;
      }
    /* Misc. */
    while (uart0_poll ())
	proto_accept (uart0_getc ());
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
	pos_reset ();
	main_mode = 0;
	pwm_left = 0;
	pwm_right = 0;
	break;
      case c ('w', 4):
	/* Set pwm.
	 * - w: left pwm.
	 * - w: right pwm. */
	pos_reset ();
	main_mode = 0;
	pwm_left = v8_to_v16 (args[0], args[1]);
	UTILS_BOUND (pwm_left, -PWM_MAX, PWM_MAX);
	pwm_right = v8_to_v16 (args[2], args[3]);
	UTILS_BOUND (pwm_right, -PWM_MAX, PWM_MAX);
	break;
      case c ('c', 4):
	/* Add to position consign.
	 * - w: theta consign offset.
	 * - w: alpha consign offset. */
	main_mode = 1;
	pos_theta_cons += v8_to_v16 (args[0], args[1]);
	pos_alpha_cons += v8_to_v16 (args[2], args[3]);
	break;
      case c ('s', 0):
	/* Stop (set zero speed). */
	main_mode = 2;
	speed_pos = 0;
	speed_theta_cons = 0;
	speed_alpha_cons = 0;
	break;
      case c ('s', 2):
	/* Set speed.
	 * - b: theta speed.
	 * - b: alpha speed. */
	main_mode = 2;
	speed_pos = 0;
	speed_theta_cons = args[0] << 8;
	speed_alpha_cons = args[1] << 8;
	break;
      case c ('s', 9):
	/* Set speed controlled position consign.
	 * - d: theta consign offset.
	 * - d: alpha consign offset.
	 * - b: sequence number. */
	if (args[8] == main_sequence)
	    break;
	main_mode = 2;
	speed_pos = 1;
	speed_theta_pos_cons = pos_theta_cons;
	speed_theta_pos_cons += v8_to_v32 (args[0], args[1], args[2], args[3]);
	speed_alpha_pos_cons = pos_alpha_cons;
	speed_alpha_pos_cons += v8_to_v32 (args[4], args[5], args[6], args[7]);
	main_sequence = args[8];
	break;
      case c ('f', 1):
	/* Go to the wall.
	 * - b: sequence number. */
	if (args[0] == main_sequence)
	    break;
	main_mode = 3;
	speed_pos = 0;
	traj_mode = 10;
	main_sequence = args[0];
	break;
      case c ('a', 1):
	/* Set acknoledge.
	 * - b: ack sequence number. */
	main_sequence_ack = args[0];
	if (pos_blocked_state)
	    pos_reset ();
	break;
    /* Stats.
     * - b: interval between stats. */
      case c ('C', 1):
	/* Counter stats. */
	main_stat_counter_cpt = main_stat_counter = args[0];
	break;
      case c ('X', 1):
	/* Position stats. */
	main_stat_postrack_cpt = main_stat_postrack = args[0];
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
      case c ('Y', 1):
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
	      case c ('f', 3):
		/* Set footing.
		 * - w: footing. */
		postrack_set_footing (v8_to_v16 (args[1], args[2]));
		break;
	      case c ('a', 5):
		/* Set acceleration.
		 * - w: theta.
		 * - w: alpha. */
		speed_theta_acc = v8_to_v16 (args[1], args[2]);
		speed_alpha_acc = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('s', 5):
		/* Set maximum and slow speed.
		 * - b: theta max.
		 * - b: alpha max.
		 * - b: theta slow.
		 * - b: alpha slow. */
		speed_theta_max = args[1];
		speed_alpha_max = args[2];
		speed_theta_slow = args[3];
		speed_alpha_slow = args[4];
		break;
	      case c ('p', 3):
		pos_theta_kp = pos_alpha_kp = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('p', 5):
		pos_theta_kp = v8_to_v16 (args[1], args[2]);
		pos_alpha_kp = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('i', 3):
		pos_theta_ki = pos_alpha_ki = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('i', 5):
		pos_theta_ki = v8_to_v16 (args[1], args[2]);
		pos_alpha_ki = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('d', 3):
		pos_theta_kd = pos_alpha_kd = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('d', 5):
		pos_theta_kd = v8_to_v16 (args[1], args[2]);
		pos_alpha_kd = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('E', 3):
		pos_e_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('I', 3):
		pos_int_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('b', 3):
		pos_blocked = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('w', 3):
		/* Set PWM direction.
		 * - b: inverse left direction.
		 * - b: inverse right direction. */
		pwm_dir = 0;
		if (args[1]) pwm_dir |= _BV (PWM_LEFT_DIR);
		if (args[2]) pwm_dir |= _BV (PWM_RIGHT_DIR);
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
		proto_send1w ('f', postrack_footing);
		proto_send2w ('a', speed_theta_acc, speed_alpha_acc);
		proto_send2b ('s', speed_theta_max, speed_alpha_max);
		proto_send2w ('p', pos_theta_kp, pos_alpha_kp);
		proto_send2w ('i', pos_theta_ki, pos_alpha_ki);
		proto_send2w ('d', pos_theta_kd, pos_alpha_kd);
		proto_send1w ('E', pos_e_sat);
		proto_send1w ('I', pos_int_sat);
		proto_send1b ('w', pwm_dir);
		break;
	      default:
		proto_send0 ('?');
		return;
	      }
	  }
#ifdef HOST
	else if (cmd == 'y')
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

