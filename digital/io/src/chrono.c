/* chrono.c */
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

#include "aquajim.h"
#include "main_timer.h"
#include "asserv.h"

#include "modules/utils/utils.h"
#include "modules/host/mex.h"

#include "chrono.h"

/**
 * Implementation notes.
 * This module compute the number of tic of the main loop it should count
 * before the match is over (chrono_init). Every tic of the main loop, it
 * decrements the counter (chrono_update). When the counter is zero, the
 * match is over (chrono_is_match_over, chrono_end_match).
 */

/**
 * Number of overflows of the timer/counter 0 to wait before the match is
 * over.
 * Basically, it is match_duration / timer_counter_0_overflow_duration.
 */
#define CHRONO_MATCH_OVERFLOW_COUNT (MATCH_DURATION_MS / MT_TC0_PERIOD)

/**
 * Duration of a loop to emulate from the original behaviour, in ms.
 */
#define CHRONO_LOOP_DURATION_MS 4

/**
 * Time to wait before resetting asserv board, in ms.
 */
#define CHRONO_WAIT_BEFORE_RESET_MS 1000

/**
 * Number of time to overflow before the end of the match.
 */
static uint32_t chrono_ov_count_;

/**
 * Status of the chrono module.
 * Set to 0 if the module is disabled, otherwise set to a non 0 value.
 */
static uint8_t chrono_enabled_ = 0;


void
chrono_init (void)
{
    /* Enable chrono. */
    chrono_enable ();
    /* Set the overflow counter to the maximum of overflow before the end of
     * the match. */
    chrono_ov_count_ = CHRONO_MATCH_OVERFLOW_COUNT;
}

void
chrono_update (void)
{
    /* Decrement overflow counter if it is possible. */
    if (chrono_enabled_ && chrono_ov_count_)
	chrono_ov_count_--;
}

uint8_t
chrono_is_match_over (void)
{
    if (chrono_ov_count_)
	return 0;
    else
	return 1;
}

void
chrono_enable (void)
{
    chrono_enabled_ = 1;
}

void
chrono_disable (void)
{
    chrono_enabled_ = 0;
}

uint8_t
chrono_enabled (void)
{
    return chrono_enabled_;
}

uint32_t
chrono_remaining_time (void)
{
    return chrono_ov_count_ * MT_TC0_PERIOD;
}

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
	utils_delay_ms (CHRONO_LOOP_DURATION_MS);
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
	    utils_delay_ms (CHRONO_LOOP_DURATION_MS);
	  }
	else
	    /* Exit loop */
	    break;
      }

    /* Wait CHRONO_WAIT_BEFORE_RESET ms before reseting */
    utils_delay_ms (CHRONO_WAIT_BEFORE_RESET_MS);
    /* Reset the asserv board */
    asserv_reset ();
    /* Block indefinitely */
    if (block)
	while (42)
#ifdef HOST
	  {
	    mex_node_wait ();
	  }
#else
	    ;
#endif
}
