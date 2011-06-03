#ifndef element_h
#define element_h
/* element.h */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet, Jérôme Jutteau
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

/** None. */
#define ELEMENT_NONE 1
/** A pawn, not a head. */
#define ELEMENT_PAWN 2
/** Queen pawn, used for statistic update of table data. */
#define ELEMENT_QUEEN 4
/** King pawn, used for statistic update of table data. */
#define ELEMENT_KING 8
/** Any head, queen or king. */
#define ELEMENT_HEAD (ELEMENT_QUEEN |ELEMENT_KING)
/** Any element, pawn, queen, or king. */
#define ELEMENT_ANY (ELEMENT_HEAD |ELEMENT_PAWN)
/** Tower types. */
#define ELEMENT_TOWER_1_QUEEN 16
#define ELEMENT_TOWER_2_QUEEN 32
#define ELEMENT_TOWER_1_KING 64
#define ELEMENT_TOWER_2_KING 128

#define ELEMENT_TOWER (ELEMENT_TOWER_1_QUEEN | \
		       ELEMENT_TOWER_2_QUEEN | \
		       ELEMENT_TOWER_1_KING | \
		       ELEMENT_TOWER_2_KING)

/** Return non zero if element is a head, not a pawn. */
#define ELEMENT_IS_HEAD(e) ((e) && !((e) & ELEMENT_PAWN))

/** Return non zero if element may be a head. */
#define ELEMENT_CAN_BE_HEAD(e) ((e) & ELEMENT_HEAD)

#define ELEMENT_PAWN_SCORE 10
#define ELEMENT_ANY_SCORE 15
#define ELEMENT_QUEEN_SCORE 20 * 30
#define ELEMENT_KING_SCORE 30 * 30

/** Emplacement attributes bits. */
#define ELEMENT_BONUS 1
/**
 * Two meanings:
 * - When it is on an intersection or in the green zone, this mean that it is positioned on the
 *   left size of the table.
 * - When it is located on an color, this corresponds to the red color.
 */
#define ELEMENT_LEFT 2
/**
 * Two meanings:
 * - When it is on an intersection or in the green zone, this mean that it is positioned on the
 *   left right of the table.
 * - When it is located on an color, this corresponds to the blue color.
 */
#define ELEMENT_RIGHT 4
#define ELEMENT_SAFE 8
#define ELEMENT_GREEN 16
#define ELEMENT_INTERSEC 64
/** Center of a square or central element. */
#define ELEMENT_CENTER 128

struct element_t
{
    /** May have several types if unknown (pawn, king, queen). */
    uint8_t type;
    /** Element position. */
    vect_t pos;
    /** Emplacement attributes. */
    uint8_t attr;
    /** Bonus coefficient (or Mallus if negative) for load scores. */
    int8_t bonus_load;
    /** Bonus coefficient (or Mallus if negative) for unload scores. */
    int8_t bonus_unload;
    /** Failure expiration date, can not take this element until chrono is
     * lower than this date in seconds. */
    int8_t failure_until_s;

};
typedef struct element_t element_t;

/* Elements range, see element table content. */
#define ELEMENT_INTERSEC_START 0
#define ELEMENT_INTERSEC_END 19
#define ELEMENT_CENTRAL_PAWN 20
#define ELEMENT_GREEN_START 21
#define ELEMENT_GREEN_END 30
#define ELEMENT_UNLOAD_START 31
#define ELEMENT_UNLOAD_END 66

#define ELEMENT_BONUS_COEFF 100

/** Elements on table. */
extern struct element_t element_table[];

/** Initialize elements. */
void
element_init (void);

/** Gives the score of an element considering it as an unload zone. */
int32_t
element_unload_score (position_t robot_pos, uint8_t element_id);

/** Gives best unload. */
uint8_t
element_unload_best (position_t robot_pos);

/** Gives score of an element. */
int32_t
element_score (position_t robot_pos, uint8_t element_id);

/** Return a probability of an element to be here.
  * This depends of the remaining time.
  * Returns a probability from 0 (element may really not be here)
  * to maximal value (element may be here).
  */
uint32_t
element_proba (uint8_t element_id);

/** Return the best element to pick. */
uint8_t
element_best (position_t robot_pos);

/** Function to call when we see an element is not here. */
void
element_not_here (uint8_t element_id);

/** Call this function when an element is taken. */
void
element_taken (uint8_t element_id, uint8_t element_type);

/** Call this function when the robot put down an element. */
void
element_down (uint8_t element_id, uint8_t element_type);

/** Call this function when taking an element failled. */
void
element_failure (uint8_t element_id);

/** Gives the nearest element from a position. */
uint8_t
element_give_position (position_t pos);

/** Give the opposed element. */
uint8_t
element_opposed (uint8_t element_id);

/** Return 1 if the element is in our side, 0 otherwise. */
int
element_our_side (uint8_t element_id);

inline void
element_intersec_symetric (uint8_t element_id, uint8_t element_type);

uint8_t
element_nearest_element_id (position_t robot_pos);

/**
   Give the position where the robot need to be placed to get an element.
 */
vect_t
element_get_pos (uint8_t element_id);

extern inline element_t
element_get (uint8_t element_id)
{
    return element_table[element_id];
}

/** Return whether an element is blocking robot. */
uint8_t
element_blocking (uint8_t element_id);

/** Return whether an element is blocking a line segment.
 * - a: line segment first point.
 * - b: line segment second point.
 * - ab: line segment length.
 * - returns: 1 if the path should not be used. */
uint8_t
element_blocking_path (vect_t a, vect_t b, int16_t ab);

/** Ask to adjust score for the opposed green zone. */
void
element_i_like_green ();

#endif /* element_h */
