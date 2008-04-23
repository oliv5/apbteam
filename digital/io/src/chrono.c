/* chrono.avr.c */
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

#include "chrono.h"

#include "modules/utils/utils.h"	/* regv */
#include "asserv.h"			/* asserv_* */

#include "io.h"				/* Registers for timer/counter 1 */

/**
 * Number of overflow of the timer/counter 1 before doing the last one.
 */
#define CHRONO_OVERFLOW_MAX 70

/**
 * Number of TIC to restart from for the last overflow.
 */
#define CHRONO_RESTART_TIC 58982
/**
 * Duration of a loop to emulate from the original behaviour, in ms.
 */
#define CHRONO_LOOP_DURATION 4
/**
 * Time to wait before resetting asserv board, in ms.
 */
#define CHRONO_WAIT_BEFORE_RESET 1000

/**
 * Match is finished.
 * This variable will be set to 1 when the match is over.
 */
static volatile uint8_t chrono_match_over_ = 0;

/**
 * Overflow counter.
 */
static volatile uint8_t chrono_ov_count_;

/* Initialize the chrono timer/counter 1. */
void
chrono_init (void)
{
#ifndef HOST
    /* Presaler = 256 */
    TCCR1B = regv (ICNC1, ICES1, 5, WGM13, WGM12, CS12, CS11, CS10,
		       0,     1, 0,     0,     0,    1,    0,    0);
    /* Enable overflow interrupt */
    set_bit (TIMSK, TOIE1);
#endif
}

#ifndef HOST
/* Overflow of timer/counter 1 handler. */
SIGNAL (SIG_OVERFLOW1)
{
    switch (++chrono_ov_count_)
      {
      case CHRONO_OVERFLOW_MAX:
	/* Last but not complete overflow */
	TCNT1 = CHRONO_RESTART_TIC;
	break;
      case CHRONO_OVERFLOW_MAX + 1:
	/* End of match! */
	chrono_match_over_ = 1;
	break;
      }
}
#endif

/* Match over? */
uint8_t
chrono_is_match_over (void)
{
    return chrono_match_over_;
}

/* End the match. */
void
chrono_end_match (uint8_t block)
{
    /* Make sure previous command has been acknowledged. If not, retransmit
     * until acknowledged */
    while (asserv_last_cmd_ack () == 0)
      {
	/* Update status */
	asserv_update_status ();
	/* Manage retransmission */
	asserv_retransmit ();
	/* Wait a little */
	utils_delay_ms (CHRONO_LOOP_DURATION);
      }

    /* Make the bot stop moving */
    asserv_stop_motor ();

    /* Wait until complete */
    while (42)
      {
	/* Update the asserv board */
	asserv_update_status ();
	/* Stop acknowledged ? */
	if (asserv_last_cmd_ack () == 0)
	  {
	    /* Retransmit if needed */
	    asserv_retransmit ();
	    /* Wait a little */
	    utils_delay_ms (CHRONO_LOOP_DURATION);
	  }
	else
	    /* Exit loop */
	    break;
      }

    /* Wait CHRONO_WAIT_BEFORE_RESET ms before reseting */
    utils_delay_ms (CHRONO_WAIT_BEFORE_RESET);
    /* Reset the asserv board */
    asserv_reset ();
    /* Block indefinitely */
    if (block)
	while (42);
}
