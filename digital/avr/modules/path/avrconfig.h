#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h - Path module configuration template. */
/* avr.path - Path finding module. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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

/* path - Path finding module. */
/** Report path found for debug. */
#define AC_PATH_REPORT defined (HOST)
/** Report function name. */
#define AC_PATH_REPORT_CALLBACK path_report
/** Number of possible obstacles. */
#define AC_PATH_OBSTACLES_NB 2
/** Number of points per obstacle. */
#define AC_PATH_OBSTACLES_POINTS_NB 8

#endif /* avrconfig_h */
