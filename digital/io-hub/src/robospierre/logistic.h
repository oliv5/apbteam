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

/** Defines for logistic macro, see LOGISTIC_CASE. */
/** Any element must be present (not empty). */
#define LOG_a 0
/** Do not care if there is an element or not. */
#define LOG__ 1
/** There must not be element here. */
#define LOG_e 2
/** Pawn to pick up. */
#define LOG_P 4
/** A head to pick up. */
#define LOG_H 5
/** Pawn who has to be present. */
#define LOG_p 6
/** Head who has to be present. */
#define LOG_h 7
/** Destination (who has to be empty). */
#define LOG_D 8
/** We have a tower (always at bottom). */
#define LOG_t 9
/* LEFT means we keep the same side, RIGHT means we put the opposed side. */
#define LOG_DIR_LEFT 0
#define LOG_DIR_RIGHT 1
/** Logistic macro to test if a case correspond and to perform movement.
  * We have to consider that the moving direction is on the left.
  * New direction is indicated (same or opposed).
  * Set ready or not. */
#define LOGISTIC_CASE(e1,       e2, \
		      e3,  e4,  e5, \
		      e6,       e7, new_dir, ready) \
{ \
    if (logistic_case (LOG_##e1, LOG_##e2, LOG_##e3, LOG_##e4, LOG_##e5, \
		       LOG_##e6, LOG_##e7, LOG_DIR_##new_dir, ready, 0)) \
    return; \
}

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
    /** Inform TOP if no construct is possible (0), if a tower is possible (1), if an
     * other element can be put even if the clamp is broken (2). */
    uint8_t construct_possible;
    /** TOP set 0 if we do not want to build something, 1 if we can build a tower, 2 to prepare
      anything possible, 3 the clamp is broken, prepare anything with *_BOTTOM. */
    uint8_t prepare;
    /** Inform TOP if a construct is ready (1) or not (0). */
    uint8_t ready;
    /* Inform TOP that we can't take any more elements and needs to put
     * construction somewhere. */
    uint8_t need_prepare;
    /** Filtered side slot sensor. */
    uint8_t side_state;
    /** Filtered side slot sensor counter. */
    uint16_t side_filter;
};

/** Global context. */
extern struct logistic_t logistic_global;

/** Initialise module. */
void
logistic_init (void);

/** Examine current state and take a decision. */
void
logistic_decision (void);

/** To be called at regular interval to check for bad robot state. */
void
logistic_update (void);

/** To be called when a new element is entering the robot. */
void
logistic_element_new (uint8_t pos, uint8_t element_type);

/** Oh la la, the pawn was not a pawn, it's a head. */
void
logistic_element_change (uint8_t pos, uint8_t element_type);

/** To be called when a element movement is done. */
void
logistic_element_move_done (void);

/** To be called when elements have been dropped on the opposite side of
 * direction. */
void
logistic_drop (uint8_t direction);

/** Get element type to be dropped. */
uint8_t
logistic_drop_element_type (uint8_t direction);

/** Dump every element. */
void
logistic_dump (void);

/** Is path clear between two positions? */
uint8_t
logistic_path_clear (uint8_t slot1, uint8_t slot2);

#endif /* logistic_h */
