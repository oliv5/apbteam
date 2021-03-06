/* radar_defs.c */
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
#include "common.h"
#include "radar.h"

#include "modules/devices/usdist/usdist.h"
#include "playground_2010.h"

/** Define radar configuration. */
struct radar_sensor_t radar_sensors[RADAR_SENSOR_NB] = {
#define RADAR_SENSOR_FRONT 0
      { &usdist_mm[0], { 30 - 20, 0 }, G_ANGLE_UF016_DEG (0) },
#define RADAR_SENSOR_LEFT 1
      { &usdist_mm[1], { 20 - 20, 20 }, G_ANGLE_UF016_DEG (30) },
#define RADAR_SENSOR_RIGHT 2
      { &usdist_mm[2], { 20 - 20, -20 }, G_ANGLE_UF016_DEG (-30) },
#define RADAR_SENSOR_BACK 3
      { &usdist_mm[3], { -30 - 20, 0 }, G_ANGLE_UF016_DEG (180) },
};

/** Define exclusion area (considered as invalid point). */
uint8_t
radar_valid (vect_t p, uint8_t sensor)
{
    return p.x >= RADAR_MARGIN_MM && p.x < PG_WIDTH - RADAR_MARGIN_MM
	&& p.y >= RADAR_MARGIN_MM && p.y < PG_LENGTH - RADAR_MARGIN_MM
	/* Ignore points on slope, no margin for the slope start. */
	&& (p.x < PG_WIDTH / 2 - PG_SLOPE_WIDTH / 2
	    || p.x >= PG_WIDTH / 2 + PG_SLOPE_WIDTH / 2
	    || p.y < PG_LENGTH - PG_SLOPE_LENGTH - RADAR_MARGIN_MM / 2);
}

