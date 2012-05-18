#ifndef radar_defs_h
#define radar_defs_h
/* radar_defs.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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

#define RADAR_OBSTACLE_EDGE_RADIUS_MM 150
#define RADAR_OBSTACLE_RADIUS_MM 150
#define RADAR_STOP_MM 350
#define RADAR_CLEARANCE_MM 100
#define RADAR_EPSILON_MM 70

#define RADAR_SENSOR_NB 4

#define RADAR_SENSOR_FRONT_FIRST 0
#define RADAR_SENSOR_FRONT_NB 3
#define RADAR_SENSOR_BACK_FIRST 3
#define RADAR_SENSOR_BACK_NB 1

void
radar_def_upper_clamp_moving (uint8_t moving);

#endif /* radar_defs_h */
