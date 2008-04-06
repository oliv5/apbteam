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

#include "modules/math/fixed/fixed.h"
#include "modules/math/math.h"
#include "io.h"

#include "state.h"

#include "pos.h"
#include "speed.h"
#include "postrack.h"

#ifdef HOST
# include "simu.host.h"
#endif

/* This file provides high level trajectories commands. */

/** Traj mode enum. */
enum
{
    /* Go to the wall. */
    TRAJ_FTW,
    /* Go to the dispenser. */
    TRAJ_GTD,
    /* Everything done. */
    TRAJ_DONE,
};

/** Traj mode. */
uint8_t traj_mode;

/** Angle offset.  Directly handled to speed layer. */
void
traj_angle_offset_start (int32_t angle, uint8_t seq)
{
    int32_t a = fixed_mul_f824 (angle, 2 * M_PI * (1L << 24));
    uint32_t f = postrack_footing;
    int32_t arc = fixed_mul_f824 (f, a);
    state_main.mode = MODE_SPEED;
    speed_theta.use_pos = speed_alpha.use_pos = 1;
    speed_theta.pos_cons = pos_theta.cons;
    speed_alpha.pos_cons = pos_alpha.cons;
    speed_alpha.pos_cons += arc;
    state_start (&state_main, seq);
}

/** Go to the wall mode. */
static void
traj_ftw (void)
{
    int16_t speed;
    speed = speed_theta.slow;
    speed *= 256;
    speed_theta.use_pos = speed_alpha.use_pos = 0;
    if (PINC & _BV (0) && PINC & _BV (1))
      {
	speed_theta.cons = -speed;
	speed_alpha.cons = 0;
      }
    else if (PINC & _BV (0))
      {
	speed_theta.cons = -speed / 2;
	speed_alpha.cons = speed / 2;
      }
    else if (PINC & _BV (1))
      {
	speed_theta.cons = -speed / 2;
	speed_alpha.cons = -speed / 2;
      }
    else
      {
	speed_theta.cons = 0;
	speed_alpha.cons = 0;
	speed_theta.cur = 0;
	speed_alpha.cur = 0;
	state_finish (&state_main);
	traj_mode = TRAJ_DONE;
      }
}

/** Start go to the wall mode. */
void
traj_ftw_start (uint8_t seq)
{
    state_main.mode = MODE_TRAJ;
    traj_mode = TRAJ_FTW;
    state_start (&state_main, seq);
}

/** Go to the dispenser mode. */
static void
traj_gtd (void)
{
    int16_t speed;
    speed = speed_theta.slow;
    speed *= 256;
    speed_theta.use_pos = speed_alpha.use_pos = 0;
    if (PINC & _BV (2))
      {
	speed_theta.cons = speed;
	speed_alpha.cons = 0;
      }
    else
      {
	speed_theta.cons = 0;
	speed_alpha.cons = 0;
	speed_theta.cur = 0;
	speed_alpha.cur = 0;
	state_finish (&state_main);
	traj_mode = TRAJ_DONE;
      }
}

/** Start go to the dispenser mode. */
void
traj_gtd_start (uint8_t seq)
{
    state_main.mode = MODE_TRAJ;
    traj_mode = TRAJ_GTD;
    state_start (&state_main, seq);
}

/* Compute new speed according the defined trajectory. */
void
traj_update (void)
{
    switch (traj_mode)
      {
      case TRAJ_FTW:
	traj_ftw ();
	break;
      case TRAJ_GTD:
	traj_gtd ();
	break;
      case TRAJ_DONE:
	break;
      }
}

