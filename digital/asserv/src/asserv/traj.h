#ifndef traj_h
#define traj_h
/* traj.h */
/* asserv - Position & speed motor control on AVR. {{{
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

#define TRAJ_BACKWARD 1
#define TRAJ_REVERT_OK 2

extern uint8_t traj_mode;
extern int16_t traj_eps;
extern int16_t traj_aeps;
extern uint16_t traj_angle_limit;

void
traj_init (void);

void
traj_update (void);

void
traj_angle_offset_start (int32_t angle, uint8_t seq);

void
traj_ftw_start (uint8_t backward, uint8_t seq);

void
traj_ftw_start_center (uint8_t backward, uint8_t center_delay, uint8_t seq);

void
traj_ptw_start (uint8_t backward, int32_t init_x, int32_t init_y,
		int32_t init_a, uint8_t seq);

void
traj_gtd_start (uint8_t seq);

void
traj_goto_start (uint32_t x, uint32_t y, uint8_t backward, uint8_t seq);

void
traj_goto_angle_start (uint32_t a, uint8_t seq);

void
traj_goto_xya_start (uint32_t x, uint32_t y, uint32_t a, uint8_t backward,
		     uint8_t seq);

void
traj_set_angle_limit (uint16_t a);

#endif /* traj_h */
