#ifndef strat_h
#define strat_h
/* strat.h */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "defs.h"

enum
{
    /** Go collect items on a totem side. */
    STRAT_DECISION_TOTEM,
    /** Go push a bottle button. */
    STRAT_DECISION_BOTTLE,
    /** Go unload everything in our boat. */
    STRAT_DECISION_UNLOAD,
};

/** Initialise strategy once color is known. */
void
strat_init (void);

/** Take a decision, return its code. */
uint8_t
strat_decision (vect_t *pos);

/** Take a decision in advance so that next strat_decision call is
 * immediate. */
void
strat_prepare (void);

/** Report a success of last decision. */
void
strat_success (void);

/** Report a failure of last decision. */
void
strat_failure (void);

/** Report a really bad failure. */
void
strat_bad_failure (void);

/** Report a failure of last decision, never try again. */
void
strat_giveup (void);

/** Signal a coin has been taken. */
void
strat_coin_taken (void);

/** Clamp is dead stop using it. */
void
strat_clamp_dead (void);

/** Upper clamp is dead. */
void
strat_upper_clamp_dead (void);

#endif /* strat_h */
