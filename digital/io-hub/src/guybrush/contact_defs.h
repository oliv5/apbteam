#ifndef contact_defs_h
#define contact_defs_h
/* contact_defs.h */
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

#define CONTACT_COLOR E, 3
#define CONTACT_JACK E, 5
#define CONTACT_STRAT E, 1
#define CONTACT_LOWER_CLAMP_SENSOR_1 D, 5
#define CONTACT_LOWER_CLAMP_SENSOR_2 B, 6
#define CONTACT_LOWER_CLAMP_SENSOR_3 D, 6
#define CONTACT_LOWER_CLAMP_SENSOR_4 B, 7
#define CONTACT_UPPER_CLAMP_DOWN E, 4
#define CONTACT_UPPER_CLAMP_UP E, 5
#define CONTACT_DOOR_OPEN E, 0
#define CONTACT_DOOR_CLOSE E, 1
#define CONTACT_GOLD_BAR E, 6
#define CONTACT_TOTEM E, 7

#define CONTACT_LIST \
    CONTACT (CONTACT_STRAT, 1) \
    CONTACT (CONTACT_LOWER_CLAMP_SENSOR_1, 0) \
    CONTACT (CONTACT_LOWER_CLAMP_SENSOR_2, 0) \
    CONTACT (CONTACT_LOWER_CLAMP_SENSOR_3, 0) \
    CONTACT (CONTACT_LOWER_CLAMP_SENSOR_4, 0) \
    CONTACT (CONTACT_UPPER_CLAMP_DOWN, 1) \
    CONTACT (CONTACT_UPPER_CLAMP_UP, 1) \
    CONTACT (CONTACT_DOOR_OPEN, 1) \
    CONTACT (CONTACT_DOOR_CLOSE, 1) \
    CONTACT (CONTACT_GOLD_BAR, 1) \
    CONTACT (CONTACT_TOTEM, 1)

#endif /* contact_defs_h */
