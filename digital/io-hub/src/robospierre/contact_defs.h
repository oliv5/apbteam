#ifndef contact_defs_h
#define contact_defs_h
/* contact_defs.h */
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

#define CONTACT_COLOR E, 5
#define CONTACT_JACK E, 6
#define CONTACT_STRAT E, 5
#define CONTACT_FRONT_BOTTOM A, 4
#define CONTACT_FRONT_MIDDLE F, 4
#define CONTACT_BACK_BOTTOM A, 5
#define CONTACT_BACK_MIDDLE F, 5
#define CONTACT_FRONT_TOP A, 6
#define CONTACT_BACK_TOP F, 6
#define CONTACT_SIDE E, 7

#define CONTACT_LIST \
    CONTACT (CONTACT_FRONT_BOTTOM) \
    CONTACT (CONTACT_FRONT_MIDDLE) \
    CONTACT (CONTACT_FRONT_TOP) \
    CONTACT (CONTACT_BACK_BOTTOM) \
    CONTACT (CONTACT_BACK_MIDDLE) \
    CONTACT (CONTACT_BACK_TOP) \
    CONTACT (CONTACT_SIDE) \
    CONTACT (CONTACT_STRAT)

#endif /* contact_defs_h */
