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
 *
 * It is based on the main timer (time/counter 0) to know when to stop the
 * bot.
 *
 * The main loop should never last more than the 4.44ms defined, otherwise,
 * this module will not be precise at all!
 */

/**
 * Initialize the chrono module.
 * It setups it for a duration of MATCH_DURATION_MS.
 */
void
chrono_init (void);

/**
 * Update chrono module.
 * You must call this function every overflow of the main timer.
 */
void
chrono_update (void);

/**
 * Enable chrono module.
 * You should call this function when a match start.
 */
void
chrono_enable (void);

/**
 * Disable chrono module.
 */
void
chrono_disable (void);

/**
 * Is chrono module enabled?
 * @return 0 if not enabled, other values otherwise.
 */
uint8_t
chrono_enabled (void);

/**
 * Match over?
 * @return
 *   - 0 if the match is not finished yet.
 *   - 1 if the match is over.
 */
uint8_t
chrono_is_match_over (void);

/**
 * How much time remains before the end of the match.
 * @return remaining time in ms.
 */
uint32_t
chrono_remaining_time (void);

/**
 * End the match.
 * This function is responsible of resetting the asserv board to stop the bot
 * from moving and put the io board in a state where it will not do something.
 * @param block blocking function until hardware reset?
 */
void
chrono_end_match (uint8_t block);

#endif /* chrono_h */
