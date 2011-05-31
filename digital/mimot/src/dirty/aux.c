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
#include "pwm.h"

#include "contacts.h"

#ifdef HOST
# include "simu.host.h"
#endif

/** Motors states. */
struct aux_t aux[AC_ASSERV_AUX_NB];

/** Trajectory modes. */
enum
{
    /* Goto position, with blocking detection. */
    AUX_TRAJ_GOTO,
    /* Goto position, try to unblock. */
    AUX_TRAJ_GOTO_UNBLOCK,
    /* Speed control, apply open loop PWM on blocking. */
    AUX_TRAJ_CLAMP,
    /* Find zero mode, turn until zero is not seen. */
    AUX_TRAJ_FIND_ZERO_NOT,
    /* Find zero mode, turn until zero is seen. */
    AUX_TRAJ_FIND_ZERO,
    /* Find zero by forcing into limit. */
    AUX_TRAJ_FIND_LIMIT,
    /* Wait for mechanical elasticity. */
    AUX_TRAJ_FIND_LIMIT_WAIT,
    /* Everything done. */
    AUX_TRAJ_DONE,
};

/** Initialise motors states. */
void
aux_init (void)
{
    aux[0].state = &state_aux[0];
    aux[0].speed = &speed_aux[0];
    aux[0].pwm = &pwm_aux[0];
    aux[0].zero_pin = &IO_PIN (CONTACT_AUX0_ZERO_IO);
    aux[0].zero_bv = IO_BV (CONTACT_AUX0_ZERO_IO);
    aux[0].handle_blocking = 0;
    aux[1].state = &state_aux[1];
    aux[1].speed = &speed_aux[1];
    aux[1].pwm = &pwm_aux[1];
    aux[1].zero_pin = &IO_PIN (CONTACT_AUX1_ZERO_IO);
    aux[1].zero_bv = IO_BV (CONTACT_AUX1_ZERO_IO);
    aux[1].handle_blocking = 0;
}

/** Update positions. */
void
aux_pos_update (void)
{
    uint8_t i;
    /* Easy... */
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	aux[i].pos += counter_aux_diff[i];
}

/** Goto position. */
void
aux_traj_goto (struct aux_t *aux)
{
    switch (aux->traj_mode)
      {
      case AUX_TRAJ_GOTO:
	if (aux->speed->pos->blocked_counter
	    > aux->speed->pos->blocked_counter_limit)
	  {
	    aux->traj_mode = AUX_TRAJ_GOTO_UNBLOCK;
	    aux->speed->pos_cons = aux->speed->pos->cur;
	    aux->speed->pos_cons -= 250;
	    aux->wait = 225 / 2;
	  }
	else if (UTILS_ABS ((int32_t) (aux->speed->pos_cons -
				       aux->speed->pos->cur)) < 300)
	  {
	    aux->traj_mode = AUX_TRAJ_DONE;
	    aux->state->variant = 0;
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
    if (aux->handle_blocking)
	aux->state->variant = 4;
}

/** Speed control, then clamp. */
void
aux_traj_clamp (struct aux_t *aux)
{
    /* If blocking, stop control, clamp. */
    if (aux->speed->pos->blocked_counter
	> aux->speed->pos->blocked_counter_limit)
      {
	state_finish (aux->state);
	pos_reset (aux->speed->pos);
	aux->speed->cur = 0;
	aux->state->mode = MODE_PWM;
	pwm_set (aux->pwm, aux->clampin_pwm);
	aux->traj_mode = AUX_TRAJ_DONE;
      }
}

void
aux_traj_clamp_start (struct aux_t *aux, int8_t speed, int16_t clampin_pwm,
		      uint8_t seq)
{
    aux->traj_mode = AUX_TRAJ_CLAMP;
    aux->clampin_pwm = clampin_pwm;
    aux->speed->use_pos = 0;
    aux->speed->cons = speed << 8;
    state_start (aux->state, MODE_TRAJ, seq);
    aux->state->variant = 4;
}

/** Find zero mode. */
void
aux_traj_find_zero (struct aux_t *aux)
{
    uint8_t zero = *aux->zero_pin & aux->zero_bv;
    switch (aux->traj_mode)
      {
      case AUX_TRAJ_FIND_ZERO_NOT:
	if (zero)
	    aux->traj_mode = AUX_TRAJ_FIND_ZERO;
	break;
      case AUX_TRAJ_FIND_ZERO:
	if (!zero)
	  {
	    aux->speed->cons = 0;
	    state_finish (aux->state);
	    aux->pos = aux->reset_pos;
	    aux->traj_mode = AUX_TRAJ_DONE;
	  }
	break;
      }
}

/** Start find zero mode. */
void
aux_traj_find_zero_start (struct aux_t *aux, int8_t speed, int16_t reset_pos,
			  uint8_t seq)
{
    aux->traj_mode = AUX_TRAJ_FIND_ZERO_NOT;
    aux->speed->use_pos = 0;
    aux->speed->cons = speed << 8;
    state_start (aux->state, MODE_TRAJ, seq);
    aux->reset_pos = reset_pos;
}

/** Find limit mode. */
void
aux_traj_find_limit (struct aux_t *aux)
{
    switch (aux->traj_mode)
      {
      case AUX_TRAJ_FIND_LIMIT:
	/* If blocking, limit is found. */
	if (aux->speed->pos->blocked_counter
	    > aux->speed->pos->blocked_counter_limit)
	  {
	    pos_reset (aux->speed->pos);
	    aux->speed->cons = 0;
	    aux->speed->cur = 0;
	    aux->state->variant = 1;
	    pwm_set (aux->pwm, 0);
	    aux->traj_mode = AUX_TRAJ_FIND_LIMIT_WAIT;
	    aux->wait = 3 * 225;
	  }
	break;
      case AUX_TRAJ_FIND_LIMIT_WAIT:
	if (!--aux->wait)
	  {
	    state_finish (aux->state);
	    aux->pos = aux->reset_pos;
	    aux->traj_mode = AUX_TRAJ_DONE;
	  }
	break;
      }
}

/** Start find limit mode. */
void
aux_traj_find_limit_start (struct aux_t *aux, int8_t speed, int16_t reset_pos,
			   uint8_t seq)
{
    aux->traj_mode = AUX_TRAJ_FIND_LIMIT;
    aux->speed->use_pos = 0;
    aux->speed->cons = speed << 8;
    state_start (aux->state, MODE_TRAJ, seq);
    aux->state->variant = 4;
    aux->reset_pos = reset_pos;
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
	  case AUX_TRAJ_CLAMP:
	    aux_traj_clamp (aux);
	    break;
	  case AUX_TRAJ_FIND_ZERO_NOT:
	  case AUX_TRAJ_FIND_ZERO:
	    aux_traj_find_zero (aux);
	    break;
	  case AUX_TRAJ_FIND_LIMIT:
	  case AUX_TRAJ_FIND_LIMIT_WAIT:
	    aux_traj_find_limit (aux);
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
    uint8_t i;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	aux_traj_update_single (&aux[i]);
}

