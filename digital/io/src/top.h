#ifndef top_h
#define top_h
/* top.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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

#define BLUE_DISTRIBUTOR_X 700
#define BLUE_DISTRIBUTOR_Y 2100

#define RED_DISTRIBUTOR_X 2300
#define RED_DISTRIBUTOR_Y 2100

#define ICE_DISTRIBUTOR_LEFT 0 
#define ICE_DISTRIBUTOR_RIGHT 3000 
#define ICE_DISTRIBUTOR_Y 1350

enum sequence_e
{
    /* 3 color balls, 2 ice */
    SEQUCENCE_ONE = 0x15,
    /* 2 color balls, 3 ice */
    SEQUENCE_TWO = 0xA
};

struct top_data_t
{
    /** The sequence to get.
     * Each bit corresponds to the slot in the collector (where the balls a
     * stored in the robot).
     * bit 0 = slot 0
     * bit 1 = slot 1 and so on.
     */
    uint8_t sequence;
    /** The boxes already in use. */
    uint8_t boxes_used;
    /** sequence to realize. */
    uint8_t sequence_to_do;
    /** The color of the balls the robot shall take. 
     * RED_TEAM or BLUE_TEAM. */
    uint8_t team_color;
};

extern struct top_data_t top_data;

/** Start a Top FSM. */
void
top_start (uint8_t team_color);

#endif /* top_h */
