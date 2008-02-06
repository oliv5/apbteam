/* main.c */
/* mp - Position & speed motor control on AVR. {{{
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
#include "mp_pwm_LR_.h"
#include "mp_pwm_L_.h"
//#include "mp_pwm_R_.h"

/* This is implementation include. */
#ifdef HOST
# include "simu.host.h"
#endif


#ifdef HOST
/** Simulation data. */
uint8_t main_simu, main_simu_cpt;
#endif /* HOST */

/** Record timer value at different stage of computing. Used for performance
 * analisys. */
uint8_t main_timer[6];

// Left channel variables
int8_t cmd_L_;
// Right channel variables
int8_t cmd_R_;

// Environnemental variables
uint8_t temperature, battery;

// current limitation values
uint8_t curLim_soft;
uint8_t curLim_temp;
uint8_t curLim_bat;

// environemental test settings
uint16_t envTest_cpt, envTest_period, envTest_autosend;

// current limit stats
uint8_t curLim_stat_cpt, curLim_stat_period;

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
    init_timer_LR_ ();
    init_curLim ();
    //uart0_init ();
    //postrack_init ();
    envTest_period = 200;
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
    /* Uart */
    if (uart0_poll ())
	proto_accept (uart0_getc ());

    /* Counter for launching environemental tests */
    if (!(envTest_cpt --)) {
	envTest_cpt = envTest_period;
	launch_envTest ();
	curLim_temp = get_curLim_temp (temperature);
	curLim_bat= get_curLim_bat (battery);
	update_curLim ();
    }
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
	stop_motor_L_ ();
	//stop_motor_R_ ();
	utils_reset ();
	break;
	/* Commands. */
	/*
	   case c ('R', 0):
	// Set high Z state for channel R 
	stop_motor_R_ ();
	break;
	*/
      case c ('L', 0):
	/* Set high Z state for channel L */
	stop_motor_L_ ();
	break;

      case c ('l', 1):
	/* Set pwm value for _L_ side.
	 * - 0x80: -max on duty cycle (direction = 0)
	 * - 0x00: 0% on duty cycle (brake state)
	 * - 0xFF: max on duty cylcle (direction = 1) */
	cmd_L_ = (int8_t) args[0];
	if (cmd_L_ >= 0)
	  {
	    start_motor_L_(cmd_L_ , 0);
	  }
	else
	  {
	    start_motor_L_ (-cmd_L_ , 1);
	  }
	break;

	//case c ('r', 1):
	/* Set pwm value for _R_ side.
	 * - 0x80: -max on duty cycle (direction = 0)
	 * - 0x00: 0% on duty cycle (brake state)
	 * TODO : recopier ce qui est fait pour le cote _L_
	 }
	 */
	//break;

  case c ('e', 0):
	/* Get environnemental test results */
	//TODO
	// envoyer surune seule ligne :
	// - valeur batterie
	// - valeur température
	// - curLim_bat
	// - curLim_temp
	// - curLim_soft
	// - curLim
	break;

  case c ('e', 1):
	/* Set environnemental test automatic sending 
	 * 0     : no automatic send
	 * other : automatic send */
	envTest_autosend = args[0];
	break;

  case c ('E', 1):
	/* Set environnemental test period */
	envTest_cpt = envTest_period = args[0];
	break;

  case c ('c', 0):
	/* Get current limit stat */
	//TODO 
	// envoyer le nombre d'IT overcurrent qu'il y a eu
	// depuis le dernier envoi de cette stat
	break;

  case c ('c', 1):
	/* Set current limit software value */
	//TODO 
	setCurLim_soft (args[0]);
	break;

  case c ('C', 1):
	/* Set current limit stats period 
	 * Can be set to 0 for no automatic stat send */
	curLim_stat_cpt = curLim_stat_period = args[0];
	break;
	/*#ifdef HOST
	  case c ('Y', 1):
	// Simulation data.
	main_simu_cpt = main_simu = args[0];
	break;
#endif // HOST */
}
proto_send (cmd, size, args);
#undef c
}

