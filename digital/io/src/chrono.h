#ifndef chrono_h
#define chrono_h
/* chrono.h */
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

/**
 * @file Module to manage the chrono responsible to stop the bot after 90s.
 * It uses the timer/counter 1 (16 bits), configured with a prescaler set to
 * 256.
 * This way, we need to overflow 79.10071 for a match of 90s:
 * match_duration / (1 / (AC_FREQ / (Prescaler * (TOP + 1))))
 * It will overflow 79 times and them reset the timer/counter to 58982
 * (TOP - ((TOP + 1) / 10)).
 */

#include "modules/utils/utils.h"	/* regv */
#include "io.h"				/* Registers for timer/counter 1 */

/**
 * Match is finished.
 * This variable will be set to 0 when the match is over.
 */
static uint8_t chrono_match_over;

/**
 * Overflow counter.
 */
static uint8_t chrono_ov_count_;

/**
 * Initialize the chrono timer/counter 1.
 */
static inline void
chrono_init (void)
{
    /* Presaler = 256 */
    TCCR1B = regv (ICNC1, ICES1, 5, WGM13, WGM12, CS12, CS11, CS10,
		       0,     1, 0,     0,     0,    1,    0,    0);
    /* Enable overflow interrupt */
    set_bit (TIMSK, TOIE1);
}

/* Overflow of timer/counter 1 handler. */
SIGNAL (SIG_OVERFLOW1)
{
    switch (++chrono_ov_count_)
      {
      case 70:
	/* Last but not complete overflow */
	TCNT1 = 58982;
	break;
      case 71:
	/* End of match! */
	chrono_match_over = 1;
	break;
      }
}

#endif /* chrono_h */
