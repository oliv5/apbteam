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
#include "modules/utils/utils.h"
#include "io.h"

#include <math.h>

#include "state.h"

#include "pos.h"
#include "speed.h"
#include "postrack.h"

#ifdef HOST
# include "simu.host.h"
#endif

/* This file provides high level trajectories commands. */

/** Angle after which a backward movement is considered. */
#define TRAJ_GOTO_BACKWARD_THRESHOLD ((int32_t) (0.55 * M_PI * (1L << 24)))

/** The famous number. */
#define PI_F824 ((int32_t) (M_PI * (1L << 24)))

/** Traj mode enum. */
enum
{
    /* Go to the wall. */
    TRAJ_FTW,
    /* Go to the dispenser. */
    TRAJ_GTD,
    /* Go to position. */
    TRAJ_GOTO,
    /* Go to angle. */
    TRAJ_GOTO_ANGLE,
    /* Go to position, then angle. */
    TRAJ_GOTO_XYA,
    /* Everything done. */
    TRAJ_DONE,
};

/** Traj mode. */
uint8_t traj_mode;

/** Epsilon, distance considered to be small enough. */
int16_t traj_eps = 500;

/** Angle epsilon, angle considered to be small enough (f0.16). */
int16_t traj_aeps = 0x0100;

/** Angle at which to start going forward (f0.16). */
uint16_t traj_angle_limit = 0x2000;

/** Angle at which to start going forward (rad, f8.24). */
int32_t traj_angle_limit_rad;

/** Go to position. */
static uint32_t traj_goto_x, traj_goto_y;

/** Go to angle. */
static uint32_t traj_goto_a;

/** Allow backward movements. */
static uint8_t traj_backward_ok;

/** Initialise computed factors. */
void
traj_init (void)
{
    traj_set_angle_limit (traj_angle_limit);
}

/** Angle offset.  Directly handled to speed layer. */
void
traj_angle_offset_start (int32_t angle, uint8_t seq)
{
    int32_t a = fixed_mul_f824 (angle, 2 * PI_F824);
    uint32_t f = postrack_footing;
    int32_t arc = fixed_mul_f824 (f, a);
    speed_theta.use_pos = speed_alpha.use_pos = 1;
    speed_theta.pos_cons = pos_theta.cons;
    speed_alpha.pos_cons = pos_alpha.cons;
    speed_alpha.pos_cons += arc;
    state_start (&state_main, MODE_SPEED, seq);
}

/** Go to the wall mode. */
static void
traj_ftw (void)
{
    int16_t speed;
    speed = speed_theta.slow;
    speed *= 256;
    speed_theta.use_pos = speed_alpha.use_pos = 0;
    speed_theta.cons = -speed;
    speed_alpha.cons = 0;
    state_main.variant = 0;
    if (PINC & _BV (0) && PINC & _BV (1))
      {
	/* Backward. */
      }
    else if (PINC & _BV (0) || PINC & _BV (1))
      {
#ifndef HOST
	/* No angular control. */
	state_main.variant = 2;
#else
	/* On host, we must do the job. */
	speed_theta.cons = -speed / 2;
	if (PINC & _BV (0))
	    speed_alpha.cons = speed / 2;
	else
	    speed_alpha.cons = -speed / 2;
#endif
      }
    else
      {
	/* Stay here. */
	speed_theta.use_pos = speed_alpha.use_pos = 1;
	speed_theta.pos_cons = pos_theta.cur;
	speed_alpha.pos_cons = pos_alpha.cur;
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
    traj_mode = TRAJ_FTW;
    state_start (&state_main, MODE_TRAJ, seq);
}

/** Go to the dispenser mode. */
static void
traj_gtd (void)
{
    int16_t speed;
    speed = speed_theta.slow;
    speed *= 256;
    speed_theta.use_pos = speed_alpha.use_pos = 0;
    if (PINC & _BV (3))
      {
	speed_theta.cons = speed;
	speed_alpha.cons = 0;
      }
    else
      {
	/* Stay here. */
	speed_theta.use_pos = speed_alpha.use_pos = 1;
	speed_theta.pos_cons = pos_theta.cur;
	speed_alpha.pos_cons = pos_alpha.cur;
	speed_theta.cur = 0;
	speed_alpha.cur = 0;
	state_main.variant = 0;
	state_finish (&state_main);
	traj_mode = TRAJ_DONE;
      }
}

/** Start go to the dispenser mode. */
void
traj_gtd_start (uint8_t seq)
{
    traj_mode = TRAJ_GTD;
    state_start (&state_main, MODE_TRAJ, seq);
    state_main.variant = 2;
}

/** Go to position mode. */
static void
traj_goto (void)
{
    int32_t dx = traj_goto_x - postrack_x;
    int32_t dy = traj_goto_y - postrack_y;
    if (UTILS_ABS (dx) < ((int32_t) traj_eps) << 8
	&& UTILS_ABS (dy) < ((int32_t) traj_eps) << 8)
      {
	/* Near enough, stop, let speed terminate the movement. */
	state_main.mode = MODE_SPEED;
	traj_mode = TRAJ_DONE;
      }
    else
      {
	/* Projection of destination in robot base. */
	int32_t c = fixed_cos_f824 (postrack_a);
	int32_t s = fixed_sin_f824 (postrack_a);
	int32_t dt = fixed_mul_f824 (dx, c) + fixed_mul_f824 (dy, s);
	int32_t da = fixed_mul_f824 (dy, c) - fixed_mul_f824 (dx, s);
	/* Compute arc length. */
	int32_t arad = atan2 (da, dt) * (1L << 24);
	if (traj_backward_ok)
	  {
	    if (arad > TRAJ_GOTO_BACKWARD_THRESHOLD)
		arad = - PI_F824 + arad;
	    else if (arad < - TRAJ_GOTO_BACKWARD_THRESHOLD)
		arad = PI_F824 + arad;
	  }
	int32_t arc = fixed_mul_f824 (arad, postrack_footing);
	/* Compute consign. */
	speed_alpha.pos_cons = pos_alpha.cur;
	speed_alpha.pos_cons += arc;
	if (UTILS_ABS (arad) < traj_angle_limit_rad)
	  {
	    speed_theta.pos_cons = pos_theta.cur;
	    speed_theta.pos_cons += dt >> 8;
	  }
	else
	  {
	    speed_theta.pos_cons = pos_theta.cons;
	  }
      }
}

/** Start go to position mode (x, y: f24.8). */
void
traj_goto_start (uint32_t x, uint32_t y, uint8_t backward_ok, uint8_t seq)
{
    traj_mode = TRAJ_GOTO;
    traj_goto_x = x;
    traj_goto_y = y;
    traj_backward_ok = backward_ok;
    speed_theta.use_pos = speed_alpha.use_pos = 1;
    speed_theta.pos_cons = pos_theta.cons;
    speed_alpha.pos_cons = pos_alpha.cons;
    state_start (&state_main, MODE_TRAJ, seq);
}

/** Go to angle mode. */
static void
traj_goto_angle (void)
{
    /* There is some tricky parts to handle rotation direction. */
    int16_t da = (uint16_t) (traj_goto_a >> 8) - (uint16_t) (postrack_a >> 8);
    if (UTILS_ABS (da) < traj_aeps)
      {
	/* Near enough, stop, let speed terminate the movement. */
	state_main.mode = MODE_SPEED;
	traj_mode = TRAJ_DONE;
      }
    else
      {
	/* Compute arc length. */
	int32_t arad = fixed_mul_f824 (((int32_t) da) << 8,
				       2 * PI_F824);
	int32_t arc = fixed_mul_f824 (arad, postrack_footing);
	/* Compute consign. */
	speed_alpha.pos_cons = pos_alpha.cur;
	speed_alpha.pos_cons += arc;
      }
}

/** Start go to angle mode (a: f8.24). */
void
traj_goto_angle_start (uint32_t a, uint8_t seq)
{
    traj_mode = TRAJ_GOTO_ANGLE;
    traj_goto_a = a;
    speed_theta.use_pos = speed_alpha.use_pos = 1;
    speed_theta.pos_cons = pos_theta.cons;
    speed_alpha.pos_cons = pos_alpha.cons;
    state_start (&state_main, MODE_TRAJ, seq);
}

/** Go to position, then angle mode. */
static void
traj_goto_xya (void)
{
    int32_t dx = traj_goto_x - postrack_x;
    int32_t dy = traj_goto_y - postrack_y;
    if (UTILS_ABS (dx) < ((int32_t) traj_eps) << 8
	&& UTILS_ABS (dy) < ((int32_t) traj_eps) << 8)
      {
	/* Near enough, now do a go to angle. */
	traj_mode = TRAJ_GOTO_ANGLE;
	traj_goto_angle ();
      }
    else
      {
	traj_goto ();
      }
}

/** Start go to position, then angle mode (x, y: f24.8, a: f8.24). */
void
traj_goto_xya_start (uint32_t x, uint32_t y, uint32_t a, uint8_t backward_ok,
		     uint8_t seq)
{
    traj_mode = TRAJ_GOTO_XYA;
    traj_goto_x = x;
    traj_goto_y = y;
    traj_goto_a = a;
    traj_backward_ok = backward_ok;
    speed_theta.use_pos = speed_alpha.use_pos = 1;
    speed_theta.pos_cons = pos_theta.cons;
    speed_alpha.pos_cons = pos_alpha.cons;
    state_start (&state_main, MODE_TRAJ, seq);
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
      case TRAJ_GOTO:
	traj_goto ();
	break;
      case TRAJ_GOTO_ANGLE:
	traj_goto_angle ();
	break;
      case TRAJ_GOTO_XYA:
	traj_goto_xya ();
	break;
      case TRAJ_DONE:
	break;
      }
}

/* Set angle limit. */
void
traj_set_angle_limit (uint16_t a)
{
    traj_angle_limit = a;
    traj_angle_limit_rad = (uint32_t) a * (uint32_t) ((1 << 8) * M_PI * 2);
}

