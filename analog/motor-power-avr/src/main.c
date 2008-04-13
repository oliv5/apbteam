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
#include "modules/utils/utils.avr.h"
#include "modules/utils/byte.h"
#include "modules/math/fixed/fixed.h"
#include "modules/spi/spi.h"
#include "io.h"
#include "mp_pwm_LR_.h"
#include "mp_pwm_L_.h"
#include "mp_pwm_R_.h"

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

/** Currently read SPI frame. */
uint8_t spi_frame[4];
/** Current size of the received frame. */
uint8_t spi_frame_size;


/* +AutoDec */

/** Main loop. */
void main_loop (void);

/** Just do a short flash on LEDs */
void led_flash(void);

/* -AutoDec */

/** Entry point. */
int
main (int argc, char **argv)
{
    avr_init (argc, argv);

    // Show that starts
    led_flash();

    /* Pull-ups. */
    //PORTA = 0xff;

    uart0_init ();
    spi_init (SPI_IT_DISABLE | SPI_ENABLE | SPI_MSB_FIRST | SPI_SLAVE |
	      SPI_CPOL_FALLING | SPI_CPHA_SETUP | SPI_FOSC_DIV16);
    init_timer_LR_ ();
    init_curLim ();
    //postrack_init ();

    envTest_period = 200;
    proto_send0 ('z');
    sei ();

    // Background "task"
    while (1)
	    main_loop ();

    return 0;
}

/** Just do a short flash on LEDs */
void
led_flash(void)
{
    uint8_t ddrb_backup, portb_backup;

    // Save previous state
    ddrb_backup  = DDRB;
    portb_backup = PORTB;

    // Light up LEDs
    DDRB  = 0x0f;
    PORTB = 0x0f;

    // Delay 0.25s
    utils_delay(0.25);

    // Shut down LEDs
    DDRB  = 0x00;
    PORTB = 0x00;

    // Delay 0.25s
    utils_delay(0.25);

    // Restore previous state
    PORTB = portb_backup,
    DDRB  = ddrb_backup;

    return;
}

/** Main loop. */
void
main_loop (void)
{
    /* Uart */
    if (uart0_poll ())
      proto_accept (uart0_getc ());

    /* SPI. */
    if (SPSR & _BV (SPIF))
      {
	spi_frame[spi_frame_size++] = SPDR;
	if (spi_frame_size == 4)
	  {
	    /* Check integrity. */
	    if ((spi_frame[0] ^ spi_frame[1] ^ spi_frame[2] ^ spi_frame[3])
		== 0x42)
	      {
		uint8_t left_val = ((spi_frame[0] & 0x70) << 1) | (spi_frame[1] >> 3);
		uint8_t left_dir = (spi_frame[0] & 0x80) ? 1 : 0;
		if (left_dir)
		    left_val = -left_val;
		uint8_t right_val = ((spi_frame[0] & 0x07) << 5) | (spi_frame[2] >> 3);
		uint8_t right_dir = (spi_frame[0] & 0x08) ? 1 : 0;
		if (right_dir)
		    right_val = -right_val;
		start_motor_L_ (left_val, left_dir);
		start_motor_R_ (right_val, right_dir);
	      }
	    spi_frame_size = 0;
	  }
      }
    if (PINB & _BV (SPI_BIT_SS))
      {
	spi_frame_size = 0;
      }

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
	stop_motor_R_ ();
	utils_reset ();
	break;

  case c ('L', 0):
	/* Set high Z state for channel L */
	stop_motor_L_ ();
	break;

  case c ('l', 1):
	/* Set pwm value for _L_ side.
	 * - 0x00: 0% on duty cycle (brake state)
	 * - 0x7F: max on duty cycle (direction = 0)
	 * - 0x80: 0% on duty cycle (brake state)
	 * - 0xFF: max on duty cylcle (direction = 1) */
	cmd_L_ = (int8_t) args[0];
	if (cmd_L_ >= 0)
	  {
	    start_motor_L_ ((cmd_L_ << 1) , 0);
	  }
	else
	  {
	    start_motor_L_ ((cmd_L_ << 1), 1);
	  }
	break;

  case c ('R', 0):
	/* Set high Z state for channel R */
	stop_motor_R_ ();
	break;

  case c ('r', 1):
	/* Set pwm value for _R_ side.
	 * - 0x00: 0% on duty cycle (brake state)
	 * - 0x7F: max on duty cycle (direction = 0)
	 * - 0x80: 0% on duty cycle (brake state)
	 * - 0xFF: max on duty cylcle (direction = 1) */
	cmd_R_ = (int8_t) args[0];
	if (cmd_R_ >= 0)
	  {
	    start_motor_R_ ((cmd_R_ << 1) , 0);
	  }
	else
	  {
	    start_motor_R_ ((cmd_R_ << 1), 1);
	  }
	break;

  case c ('w', 0):
	/* Set zero pwm. */
    start_motor_L_ ( 0, 0);
    start_motor_R_ ( 0, 0);
	break;

  case c ('w', 4):
	/* Set pwm.
	 * - w: left pwm.
	 * - w: right pwm. */
    start_motor_L_ ( v8_to_v16 (args[0], args[1]) >> 7, (int8_t)args[0] < 0);
    start_motor_R_ ( v8_to_v16 (args[2], args[3]) >> 7, (int8_t)args[2] < 0);
	break;

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
