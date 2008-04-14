/* servo.avr.c */
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
#include "servo.h"

#include "modules/utils/utils.h"	/* regv, set_bit */
#include "modules/utils/byte.h"		/* v16_to_v8 */
#include "io.h"				/* General defines of registers */

/**
 * @defgroup ServoConfig Servo module configuration variables and defines.
 * @{
 */

/**
 * All servos are connected to the PORTA.
 */
#define SERVO_PORT PORTA
#define SERVO_DDR DDRA

/**
 * TOP of the timer/counter.
 */
#define SERVO_TCNT_TOP 0xFF

/**
 * Number of TIC the timer/counter need to do to make a whole cycle of servo
 * update.
 * It does not depend on the servo motors we manage but on the time we want a
 * whole cycle to last.
 * The formula used is:
 * time_of_a_cycle * AVR_frequency / timer_counter_prescaler
 * We want a time of 20ms (20/1000).
 * See @a servo_init to know the prescaler value of the timer/counter.
 */
static const uint16_t servo_tic_cyle_ = AC_FREQ / 256 * 20 / 1000;

/** @} */

/**
 * @defgroup ServoPrivate Servo module private variables and functions
 * declarations
 * @{
 */

/**
 * Identifier of the servo we are currently updating.
 * Note: -1 is a special value used by the servo module system to update the
 * low state of all the servos.
 */
volatile int8_t servo_updating_id_;

/**
 * A table for the time spent by each servo in high state.
 */
volatile uint8_t servo_high_time_[SERVO_NUMBER];

/**
 * Overflow of timer/counter 2 handler.
 */
SIGNAL (SIG_OVERFLOW2);

/** @} */

/* Initialize servo module. */
void
servo_init (void)
{
    /* Set-up all the pins of the servo to out direction */
    SERVO_DDR = 0xff;
    /* All pins are at low state by default */

    /* Set-up the timer/counter 2:
       - prescaler 256 => 4.44 ms TOP */
    TCCR2 = regv (FOC2, WGM20, COM21, COM20, WGM21, CS22, CS21, CS20,
		   0,    0,     0,     0,     0,    1,     0,    0);

    /* The state machine start with the first servo */
    servo_updating_id_ = 0;

    /* Enable overflow interrupt */
    set_bit (TIMSK, TOIE2);
}

/* Set the high time of the input signal of a servo (and its position). */
void
servo_set_high_time (uint8_t servo, uint8_t high_time)
{
    uint8_t filtered = high_time;
    if (filtered != 0)
	UTILS_BOUND (filtered, SERVO_HIGH_TIME_MIN, SERVO_HIGH_TIME_MAX);
    /* Sanity check */
    if (servo < SERVO_NUMBER)
	/* Set new desired position (high value time) */
	servo_high_time_[servo] = filtered;
}

/* Get the high time of the servo. */
uint8_t
servo_get_high_time (uint8_t servo)
{
    /* Sanity check */
    if (servo < SERVO_NUMBER)
	return servo_high_time_[servo];
    return 0;
}

/* Overflow of timer/counter 2 handler. */
SIGNAL (SIG_OVERFLOW2)
{
    /* Overflow count (used when we wait in the lower state).
       -1 is used for the first count where we wait less than a complete
       overflow */
    static int8_t servo_overflow_count = -1;
    /* Time spent by each servo motor at high state during a whole cycle */
    static uint16_t servo_high_time_cycle = servo_tic_cyle_;

    /* State machine actions */
    switch (servo_updating_id_)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
	/* Servos motor high state mode */

	/* Set to low state the previous servo motor pin if needed (not for
	 * the first one) */
	if (servo_updating_id_ != 0)
	    SERVO_PORT &= ~_BV (servo_updating_id_ - 1);
	/* Set to high state the current servo motor pin, unless is zero */
	if (servo_high_time_[servo_updating_id_])
	    set_bit (SERVO_PORT, servo_updating_id_);
	/* Plan next timer overflow to the TOP minus the current configuration
	 * of the servo motor */
	TCNT2 = SERVO_TCNT_TOP - servo_high_time_[servo_updating_id_];
	/* Update the time spent at high state by all servo motors for this
	 * cycle */
	servo_high_time_cycle += servo_high_time_[servo_updating_id_];
	/* Update the identifier of the current servo motor (and manage when
	 * we are at the last one) */
	if (++servo_updating_id_ == SERVO_NUMBER)
	    servo_updating_id_ = -1;
	break;

      case -1:
	/* Sleeping time mode */

	/* Is it the first we are in this mode? */
	if (servo_overflow_count == -1)
	  {
	    /* Set to low state the previous servo motor pin */
	    SERVO_PORT &= ~_BV (SERVO_NUMBER - 1);
	    /* Number of full overflow (from 0 to SERVO_TCNT_TOP) we need to
	     * wait (division by SERVO_TCNT_TOP or >> 8) */
	    servo_overflow_count = servo_high_time_cycle >> 8;
	    /* Restart the counter from remaining TIC that are left and can
	     * not be used to make a full overflow */
	    TCNT2 = SERVO_TCNT_TOP - v16_to_v8 (servo_high_time_cycle, 0);
	  }
	else
	  {
	    /* We just have an overflow, are we at the last one needed? The -1
	     * is normal: we do not count the first overflow of the sleeping
	     * mode because it is not a full one */
	    if (--servo_overflow_count == -1)
	      {
		/* Restart with first servo motor */
		servo_updating_id_ = 0;
		/* Re-initialize the counter of time spent by each servo motor
		 * at high state */
		servo_high_time_cycle = servo_tic_cyle_;
	      }
	  }
	break;
      }
}
