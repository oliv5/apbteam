/* traj.c - Trajectories. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
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
#include "traj.h"

#include "io.h"

#include "speed.h"
#include "state.h"

#ifdef HOST
# include "simu.host.h"
#endif

/* This file provides high level trajectories commands. */

/** Traj mode:
 * 10, 11: go to the wall.
 */
uint8_t traj_mode;

/** Go to the wall mode. */
static void
traj_ftw (void)
{
    int16_t speed;
    speed = speed_theta_slow;
    speed *= 256;
    if (PINC & _BV (0) && PINC & _BV (1))
      {
	speed_theta_cons = -speed;
	speed_alpha_cons = 0;
      }
    else if (PINC & _BV (0))
      {
	speed_theta_cons = -speed / 2;
	speed_alpha_cons = speed / 2;
      }
    else if (PINC & _BV (1))
      {
	speed_theta_cons = -speed / 2;
	speed_alpha_cons = -speed / 2;
      }
    else
      {
	speed_theta_cons = 0;
	speed_alpha_cons = 0;
	speed_theta_cur = 0;
	speed_alpha_cur = 0;
	state_finish (&state_main);
	traj_mode = 11;
      }
}

/* Compute new speed according the defined trajectory. */
void
traj_update (void)
{
    switch (traj_mode)
      {
      case 10:
	traj_ftw ();
      case 11:
	break;
      }
}

