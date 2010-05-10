#ifndef contacts_h
#define contacts_h
/* contacts.h */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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

/** Define contacts. */
#define CONTACT_BACK_LEFT_IO C, 0
#define CONTACT_BACK_RIGHT_IO C, 1
#define CONTACT_FRONT_LEFT_IO C, 2
#define CONTACT_FRONT_RIGHT_IO C, 3
#define CONTACT_CENTER_IO C, 4
#define CONTACT_AUX0_ZERO_IO C, 7
#define CONTACT_AUX1_ZERO_IO C, 5

#endif /* contacts_h */
