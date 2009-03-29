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

#include "modules/utils/utils.h"
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
    /* Goto position, with blocking detection. */
    AUX_TRAJ_GOTO,
    /* Goto position, try to unblock. */
    AUX_TRAJ_GOTO_UNBLOCK,
    /* Find zero mode, first start at full speed to detect a arm... */
    AUX_TRAJ_FIND_ZERO_START,
    /* ...then go on until it is not seen any more... */
    AUX_TRAJ_FIND_ZERO_SLOW,
    /* ...finally, go backward until it is seen. */
    AUX_TRAJ_FIND_ZERO_BACK,
    /* Everything done. */
    AUX_TRAJ_DONE,
};

/** Initialise motors states. */
void
aux_init (void)
{
    aux0.state = &state_aux0;
    aux0.speed = &speed_aux0;
    aux0.zero_pin = &PINC;
    aux0.zero_bv = _BV (5);
}

/** Update positions. */
void
aux_pos_update (void)
{
    /* Easy... */
    aux0.pos += counter_aux0_diff;
}

/** Goto position. */
void
aux_traj_goto (struct aux_t *aux)
{
    switch (aux->traj_mode)
      {
      case AUX_TRAJ_GOTO:
	if (aux->speed->pos->e_old > 500)
	  {
	    aux->traj_mode = AUX_TRAJ_GOTO_UNBLOCK;
	    aux->speed->pos_cons = aux->speed->pos->cur;
	    aux->speed->pos_cons -= 250;
	    aux->wait = 225 / 2;
	  }
	else if (UTILS_ABS (aux->speed->pos_cons - aux->speed->pos->cur)
		 < 300)
	  {
	    aux->traj_mode = AUX_TRAJ_DONE;
	    state_finish (aux->state);
	  }
	break;
      case AUX_TRAJ_GOTO_UNBLOCK:
	if (!--aux->wait)
	  {
	    aux->traj_mode = AUX_TRAJ_GOTO;
	    aux->speed->pos_cons = aux->goto_pos;
	  }
	break;
      }
}

void
aux_traj_goto_start (struct aux_t *aux, uint16_t pos, uint8_t seq)
{
    aux->traj_mode = AUX_TRAJ_GOTO;
    aux->speed->use_pos = 1;
    aux->speed->pos_cons = aux->speed->pos->cur;
    aux->speed->pos_cons += (int16_t) (pos - aux->pos);
    aux->goto_pos = aux->speed->pos_cons;
    state_start (aux->state, MODE_TRAJ, seq);
}

/** Find zero mode. */
void
aux_traj_find_zero (struct aux_t *aux)
{
    uint8_t zero = *aux->zero_pin & aux->zero_bv;
    switch (aux->traj_mode)
      {
	case AUX_TRAJ_FIND_ZERO_START:
	  if (!zero)
	    {
	      aux->speed->cons = aux->speed->max << 8;
	      aux->traj_mode = AUX_TRAJ_FIND_ZERO_SLOW;
	    }
	  break;
	case AUX_TRAJ_FIND_ZERO_SLOW:
	  if (zero)
	    {
	      aux->speed->cons = -aux->speed->slow << 8;
	      aux->traj_mode = AUX_TRAJ_FIND_ZERO_BACK;
	    }
	  break;
	case AUX_TRAJ_FIND_ZERO_BACK:
	  if (!zero)
	    {
	      aux->speed->cons = 0;
	      state_finish (aux->state);
	      aux->pos = 0;
	      aux->traj_mode = AUX_TRAJ_DONE;
	    }
	  break;
	default:
	  assert (0);
      }
}

/** Start find zero mode. */
void
aux_traj_find_zero_start (struct aux_t *aux, uint8_t seq)
{
    aux->traj_mode = AUX_TRAJ_FIND_ZERO_START;
    aux->speed->use_pos = 0;
    aux->speed->cons = aux->speed->max << 8;
    state_start (aux->state, MODE_TRAJ, seq);
}

/** Update trajectories for one motor. */
static void
aux_traj_update_single (struct aux_t *aux)
{
    if (aux->state->mode >= MODE_TRAJ)
      {
	switch (aux->traj_mode)
	  {
	  case AUX_TRAJ_GOTO:
	  case AUX_TRAJ_GOTO_UNBLOCK:
	    aux_traj_goto (aux);
	    break;
	  case AUX_TRAJ_FIND_ZERO_START:
	  case AUX_TRAJ_FIND_ZERO_SLOW:
	  case AUX_TRAJ_FIND_ZERO_BACK:
	    aux_traj_find_zero (aux);
	    break;
	  case AUX_TRAJ_DONE:
	    break;
	  }
      }
}

/** Update trajectories. */
void
aux_traj_update (void)
{
    aux_traj_update_single (&aux0);
}

