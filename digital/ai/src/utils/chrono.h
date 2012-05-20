#ifndef chrono_h
#define chrono_h
/* chrono.h */
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

/**
 * Module to manage the chrono responsible to stop the robot after 90s.
 */

/** Duration of a match in milliseconds, with margin. */
#define CHRONO_MATCH_DURATION_MS (90000 - 1500)

/** Start chrono count down. */
void
chrono_start (void);

/** Update chrono module. */
void
chrono_update (void);

/** Match over? Return 0 if you still have chance to make points! */
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
