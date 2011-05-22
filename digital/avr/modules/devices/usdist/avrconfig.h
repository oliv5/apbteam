#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h - TWI module configuration template. */
/* usdist - Analog US distance sensor support. {{{
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

/* usdist - Analog US distance sensor. */
/** Number of sensors. */
#define AC_USDIST_NB 4
/** Measuring period, in number of update call. */
#define AC_USDIST_PERIOD 2
/** List of space separated sensor definition, see usdist.h. */
#define AC_USDIST_SENSORS \
    USDIST_SENSOR (0, G, 3) \
    USDIST_SENSOR (1, G, 1) \
    USDIST_SENSOR (2, C, 7) \
    USDIST_SENSOR (3, D, 4)

#endif /* avrconfig_h */
