#ifndef clamp_h
#define clamp_h
/* clamp.h */
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

enum {
    /** Slot positions. */
    CLAMP_SLOT_FRONT_BOTTOM,
    CLAMP_SLOT_FRONT_MIDDLE,
    CLAMP_SLOT_FRONT_TOP,
    CLAMP_SLOT_BACK_BOTTOM,
    CLAMP_SLOT_BACK_MIDDLE,
    CLAMP_SLOT_BACK_TOP,
    CLAMP_SLOT_SIDE,
    /** Leave the front bay, ready to enter side tunnel. */
    CLAMP_BAY_FRONT_LEAVE,
    /** Leaving the front bay, entered the side tunnel.  When at midway point,
     * go directly to the next position. */
    CLAMP_BAY_FRONT_LEAVING,
    /** Leave the back bay, ready to enter side tunnel. */
    CLAMP_BAY_BACK_LEAVE,
    /** Leaving the back bay, entered the side tunnel.  When at midway point,
     * go directly to the next position. */
    CLAMP_BAY_BACK_LEAVING,
    /** Enter the side bay.  Position on the side, above wheels. */
    CLAMP_BAY_SIDE_ENTER_LEAVE,
    /** Total number of position, including intermediary positions. */
    CLAMP_POS_NB,
    /** Number of slots. */
    CLAMP_SLOT_NB = CLAMP_SLOT_SIDE + 1,
};

/** Is slot in front bay? */
#define CLAMP_IS_SLOT_IN_FRONT_BAY(slot) \
    ((slot) <= CLAMP_SLOT_FRONT_TOP)

/** Is slot in back bay? */
#define CLAMP_IS_SLOT_IN_BACK_BAY(slot) \
    ((slot) >= CLAMP_SLOT_BACK_BOTTOM && (slot) <= CLAMP_SLOT_BACK_TOP)

/** Move clamp to given position. */
void
clamp_move (uint8_t pos);

/** Move element using clamp. */
void
clamp_move_element (uint8_t from, uint8_t to);

/** Examine sensors to generate new events, return non zero if an event was
 * generated. */
uint8_t
clamp_handle_event (void);

#endif /* clamp_h */
