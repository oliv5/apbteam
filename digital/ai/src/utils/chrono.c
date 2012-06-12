/* chrono.c */
/* ai - Robot Artificial Intelligence. {{{
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

#include "timer.h"
#include "asserv.h"
#include "beacon.h"
#include "twi_master.h"

#include "modules/utils/utils.h"
#include "modules/host/mex.h"

#include "chrono.h"

/** Number of timer tick to wait before the match is over. */
#define CHRONO_MATCH_TICK_COUNT \
    (CHRONO_MATCH_DURATION_MS / TIMER_PERIOD_MS)

/** Time to wait before freeing asserv motors, in ms. */
#define CHRONO_WAIT_BEFORE_FREE_MS 1000

/** Number of timer tick left before the match ends. */
static uint32_t chrono_tick_left_;

/** Last timer tick value. */
static uint16_t chrono_last_tick_;

/** Is chrono started? */
static uint8_t chrono_started_;

void
chrono_start (void)
{
    chrono_started_ = 1;
    chrono_tick_left_ = CHRONO_MATCH_TICK_COUNT;
    chrono_last_tick_ = timer_get_tick ();
}

void
chrono_update (void)
{
    if (chrono_started_)
      {
	uint16_t new_tick = timer_get_tick ();
	uint16_t diff = new_tick - chrono_last_tick_;
	chrono_last_tick_ = new_tick;
	if (diff > chrono_tick_left_)
	    chrono_tick_left_ = 0;
	else
	    chrono_tick_left_ -= diff;
      }
}

uint8_t
chrono_is_match_over (void)
{
    if (!chrono_started_ || chrono_tick_left_)
	return 0;
    else
	return 1;
}

uint32_t
chrono_remaining_time (void)
{
    if (!chrono_started_)
	return CHRONO_MATCH_DURATION_MS;
    else
	return chrono_tick_left_ * TIMER_PERIOD_MS;
}

void
chrono_end_match (uint8_t block)
{
    vect_t v = { 0, 0 };
    /* Stop beacon system. */
    if (AC_AI_TWI_MASTER_BEACON)
      {
	beacon_on (0);
	beacon_send_position (&v);
      }
    /* Make the bot stop moving */
    asserv_stop_motor ();
    /* Wait until complete */
    while (!twi_master_sync ())
	timer_wait ();
    /* Wait, then release motors. */
    utils_delay_ms (CHRONO_WAIT_BEFORE_FREE_MS);
    asserv_free_motor ();
    /* Wait until complete */
    while (!twi_master_sync ())
	timer_wait ();
    /* Block indefinitely */
    while (block)
	utils_delay_ms (1000);
}

