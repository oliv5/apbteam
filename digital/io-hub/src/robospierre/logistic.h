#ifndef logistic_h
#define logistic_h
/* logistic.h */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "element.h"
#include "clamp.h"

/** Logistic context. */
struct logistic_t
{
    /** Current robot content.
     *
     * Elements are fully specified (pawn, queen or king).  An exception can
     * occurs when a codebar is read but does not correspond to a valid word.
     *
     * When a movement is outgoing, the element is kept in the source slot. */
    uint8_t slots[CLAMP_SLOT_NB];
    /** Current element movement source and destination, CLAMP_SLOT_NB if no
     * current movement. */
    uint8_t moving_from, moving_to;
    /** Best collect direction. */
    uint8_t collect_direction;
    /** Idle clamp position, depend on collect direction. */
    uint8_t clamp_pos_idle;
};

/** Global context. */
extern struct logistic_t logistic_global;

/** Initialise module. */
void
logistic_init (void);

/** To be called at regular interval to check for bad robot state. */
void
logistic_update (void);

/** To be called when a new element is entering the robot. */
void
logistic_element_new (uint8_t pos, uint8_t element_type);

/** To be called when a element movement is done. */
void
logistic_element_move_done (void);

/** To be called when elements have been dropped on the opposite side of
 * direction. */
void
logistic_drop (uint8_t direction);

#endif /* logistic_h */
