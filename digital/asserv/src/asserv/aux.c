/* aux.c - Auxiliary motors commands. */
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
#include "common.h"
#include "aux.h"

#include "io.h"

#include "state.h"

#include "counter.h"
#include "pos.h"
#include "speed.h"

#ifdef HOST
# include "simu.host.h"
#endif

/** Motor state. */
struct aux_t aux0;

/** Trajectory modes. */
enum
{
    /* Find zero mode, first start at full speed to detect a arm... */
    AUX_TRAJ_FIND_ZERO_START,
    /* ...then go on until it is not seen any more... */
    AUX_TRAJ_FIND_ZERO_SLOW,
    /* ...finally, go backward until it is seen. */
    AUX_TRAJ_FIND_ZERO_BACK,
    /* Everything done. */
    AUX_TRAJ_DONE,
};

/** Update positions. */
void
aux_pos_update (void)
{
    /* Easy... */
    aux0.pos += counter_aux0_diff;
}

/** Goto position. */
void
aux_traj_goto_start (uint16_t pos, uint8_t seq)
{
    speed_aux0.use_pos = 1;
    speed_aux0.pos_cons = pos_aux0.cur;
    speed_aux0.pos_cons += (int16_t) (pos - aux0.pos);
    state_start (&state_aux0, MODE_SPEED, seq);
}

/** Find zero mode. */
void
aux_traj_find_zero (void)
{
    switch (aux0.traj_mode)
      {
	case AUX_TRAJ_FIND_ZERO_START:
	  if (!(PINC & _BV (5)))
	    {
	      speed_aux0.cons = speed_aux0.max << 8;
	      aux0.traj_mode = AUX_TRAJ_FIND_ZERO_SLOW;
	    }
	  break;
	case AUX_TRAJ_FIND_ZERO_SLOW:
	  if (PINC & _BV (5))
	    {
	      speed_aux0.cons = -speed_aux0.slow << 8;
	      aux0.traj_mode = AUX_TRAJ_FIND_ZERO_BACK;
	    }
	  break;
	case AUX_TRAJ_FIND_ZERO_BACK:
	  if (!(PINC & _BV (5)))
	    {
	      speed_aux0.cons = 0;
	      state_finish (&state_aux0);
	      aux0.pos = 0;
	      aux0.traj_mode = AUX_TRAJ_DONE;
	    }
	  break;
	default:
	  assert (0);
      }
}

/** Start find zero mode. */
void
aux_traj_find_zero_start (uint8_t seq)
{
    aux0.traj_mode = AUX_TRAJ_FIND_ZERO_START;
    speed_aux0.use_pos = 0;
    speed_aux0.cons = speed_aux0.max << 8;
    state_start (&state_aux0, MODE_TRAJ, seq);
}

/** Update trajectories. */
void
aux_traj_update (void)
{
    switch (aux0.traj_mode)
      {
      case AUX_TRAJ_FIND_ZERO_START:
      case AUX_TRAJ_FIND_ZERO_SLOW:
      case AUX_TRAJ_FIND_ZERO_BACK:
	aux_traj_find_zero ();
	break;
      case AUX_TRAJ_DONE:
	break;
      }
}
