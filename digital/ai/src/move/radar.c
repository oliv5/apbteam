/* radar.c */
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

#include "bot.h"

#include "modules/math/geometry/geometry.h"
#include "modules/math/geometry/distance.h"
#include "modules/utils/utils.h"

/** Maximum distance for a sensor reading to be ignored if another sensor is
 * nearer. */
#define RADAR_FAR_MM 250

/** Define radar configuration. */
extern struct radar_sensor_t radar_sensors[RADAR_SENSOR_NB];

uint8_t
radar_valid (vect_t p);

/** Compute the center position from several radars sensors, return 1 if
 * any. */
static uint8_t
radar_hit_center (uint8_t valid[], vect_t hit[], uint8_t sensor_nb,
		  vect_t *obs_pos)
{
    uint8_t i, hit_nb = 0;
    vect_t hit_center = { 0, 0 };
    for (i = 0; i < sensor_nb; i++)
      {
	if (valid[i])
	  {
	    vect_add (&hit_center, &hit[i]);
	    hit_nb++;
	  }
      }
    if (hit_nb > 1)
	vect_scale_f824 (&hit_center, 0x1000000l / hit_nb);
    if (hit_nb)
      {
	*obs_pos = hit_center;
	return 1;
      }
    else
	return 0;
}

uint8_t
radar_update (const position_t *robot_pos, vect_t *obs_pos)
{
    uint8_t i, j;
    vect_t ray;
    uint8_t obs_nb = 0;
    /* Compute hit points for each sensor and eliminate invalid ones. */
    vect_t hit[UTILS_COUNT (radar_sensors)];
    uint8_t valid[UTILS_COUNT (radar_sensors)];
    uint16_t dist_mm[UTILS_COUNT (radar_sensors)];
    for (i = 0; i < UTILS_COUNT (radar_sensors); i++)
      {
	dist_mm[i] = *radar_sensors[i].dist_mm;
	if (dist_mm[i] != 0xffff)
	  {
	    hit[i] = radar_sensors[i].pos;
	    vect_rotate_uf016 (&hit[i], robot_pos->a);
	    vect_translate (&hit[i], &robot_pos->v);
	    vect_from_polar_uf016 (&ray, dist_mm[i],
				   robot_pos->a + radar_sensors[i].a);
	    vect_translate (&hit[i], &ray);
	    valid[i] = radar_valid (hit[i]);
	    vect_from_polar_uf016 (&ray, RADAR_OBSTACLE_EDGE_RADIUS_MM,
				   robot_pos->a + radar_sensors[i].a);
	    vect_translate (&hit[i], &ray);
	  }
	else
	    valid[i] = 0;
      }
    /* Ignore sensor results too far from other sensors. */
    for (i = 0; i < UTILS_COUNT (radar_sensors) - 1; i++)
      {
	for (j = i + 1; valid[i] && j < UTILS_COUNT (radar_sensors); j++)
	  {
	    if (valid[j])
	      {
		if (dist_mm[i] + RADAR_FAR_MM < dist_mm[j])
		    valid[j] = 0;
		else if (dist_mm[j] + RADAR_FAR_MM < dist_mm[i])
		    valid[i] = 0;
	      }
	  }
      }
    /* Specific treatment about sensor topology. */
    obs_nb += radar_hit_center (valid + RADAR_SENSOR_FRONT_FIRST,
				hit + RADAR_SENSOR_FRONT_FIRST,
				RADAR_SENSOR_FRONT_NB, &obs_pos[obs_nb]);
    obs_nb += radar_hit_center (valid + RADAR_SENSOR_BACK_FIRST,
				hit + RADAR_SENSOR_BACK_FIRST,
				RADAR_SENSOR_BACK_NB, &obs_pos[obs_nb]);
    /* Done. */
    return obs_nb;
}

uint8_t
radar_blocking (const vect_t *robot_pos, const vect_t *dest_pos,
		const vect_t *obs_pos, uint8_t obs_nb)
{
    uint8_t i;
    /* Stop here if no obstacle. */
    if (!obs_nb)
	return 0;
    vect_t vd = *dest_pos; vect_sub (&vd, robot_pos);
    uint16_t d = vect_norm (&vd);
    /* If destination is realy near, stop here. */
    if (d < RADAR_EPSILON_MM)
	return 0;
    /* If destination is near, use clearance to destination point instead of
     * stop length. */
    vect_t t;
    if (d < RADAR_STOP_MM)
	t = *dest_pos;
    else
      {
	vect_scale_f824 (&vd, (1ll << 24) / d * RADAR_STOP_MM);
	t = *robot_pos;
	vect_translate (&t, &vd);
      }
    /* Now, look at obstacles. */
    for (i = 0; i < obs_nb; i++)
      {
	/* Vector from robot to obstacle. */
	vect_t vo = obs_pos[i]; vect_sub (&vo, robot_pos);
	/* Ignore if in our back. */
	int32_t dp = vect_dot_product (&vd, &vo);
	if (dp < 0)
	    continue;
	/* Check distance. */
	int16_t od = distance_segment_point (robot_pos, &t, &obs_pos[i]);
	if (od > BOT_SIZE_SIDE + RADAR_CLEARANCE_MM / 2
	    + RADAR_OBSTACLE_RADIUS_MM)
	    continue;
	/* Else, obstacle is blocking. */
	return 1;
      }
    return 0;
}

