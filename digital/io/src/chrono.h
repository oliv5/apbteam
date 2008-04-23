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
 * @todo add the ability to unblock the chrono_end_match with a flag that can
 * be unset with an uart command. Maybe dangerous...
 */

#include "common.h"

/**
 * Initialize the chrono timer/counter 1.
 * It starts it for a duration of 90s.
 */
void
chrono_init (void);

/**
 * Match over?
 * @return
 *   - 0 if the match is not finished yet
 *   - 1 if the match is over
 */
uint8_t
chrono_is_match_over (void);

/**
 * End the match.
 * This function is responsible of resetting the asserv board to stop the bot
 * from moving and put the io board in a state where it will not do something.
 * @param block blocking function until hardware reset?
 */
void
chrono_end_match (uint8_t block);

#endif /* chrono_h */
