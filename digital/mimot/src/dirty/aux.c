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

#include "contacts.h"

#ifdef HOST
# include "simu.host.h"
#endif

/** Motors states. */
struct aux_t aux[AC_ASSERV_AUX_NB];

/** Trajectory modes. */
enum
{
    /* Everything done. */
    AUX_TRAJ_DONE,
    /* Detect end of speed controled position control. */
    AUX_TRAJ_SPEED,
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
};

/** Initialise motors states. */
void
aux_init (void)
{
    aux[0].cs = &cs_aux[0];
    aux[0].zero_pin = &IO_PIN (CONTACT_AUX0_ZERO_IO);
    aux[0].zero_bv = IO_BV (CONTACT_AUX0_ZERO_IO);
    aux[0].handle_blocking = 0;
    aux[1].cs = &cs_aux[1];
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
	aux[i].pos += aux[i].cs->encoder->diff;
}

/** Wait for zero speed mode. */
void
aux_traj_speed (struct aux_t *aux)
{
    if (aux->cs->speed.use_pos
	&& aux->cs->speed.pos_cons == aux->cs->pos.cons)
      {
	control_state_finished (&aux->cs->state);
	aux->traj_mode = AUX_TRAJ_DONE;
      }
}

/** Start speed mode. */
void
aux_traj_speed_start (struct aux_t *aux)
{
    /* Speed setup should have been done yet. */
    control_state_set_mode (&aux->cs->state, CS_MODE_TRAJ_CONTROL, 0);
    aux->traj_mode = AUX_TRAJ_SPEED;
}

/** Goto position. */
void
aux_traj_goto (struct aux_t *aux)
{
    switch (aux->traj_mode)
      {
      case AUX_TRAJ_GOTO:
	if (aux->cs->blocking_detection.blocked)
	  {
	    aux->traj_mode = AUX_TRAJ_GOTO_UNBLOCK;
	    speed_control_pos_offset_from_here (&aux->cs->speed, -250);
	    aux->wait = 225 / 2;
	  }
	else if (UTILS_ABS ((int32_t) (aux->cs->speed.pos_cons
				       - aux->cs->pos.cur)) < 300)
	  {
	    aux->traj_mode = AUX_TRAJ_DONE;
	    control_state_finished (&aux->cs->state);
	  }
	break;
      case AUX_TRAJ_GOTO_UNBLOCK:
	if (!--aux->wait)
	  {
	    aux->traj_mode = AUX_TRAJ_GOTO;
	    speed_control_pos (&aux->cs->speed, aux->goto_pos);
	  }
	break;
      }
}

void
aux_traj_goto_start (struct aux_t *aux, uint16_t pos)
{
    aux->traj_mode = AUX_TRAJ_GOTO;
    speed_control_pos_offset_from_here (&aux->cs->speed,
					(int16_t) (pos - aux->pos));
    aux->goto_pos = aux->cs->speed.pos_cons;
    control_state_set_mode (&aux->cs->state, CS_MODE_TRAJ_CONTROL,
			    aux->handle_blocking ? CS_MODE_BLOCKING_DETECTION
			    : 0);
}

/** Speed control, then clamp. */
void
aux_traj_clamp (struct aux_t *aux)
{
    /* If blocking, stop control, clamp. */
    if (aux->cs->blocking_detection.blocked)
      {
	control_state_set_mode (&aux->cs->state, CS_MODE_NONE, 0);
	control_state_finished (&aux->cs->state);
	output_set (aux->cs->output, aux->clampin_pwm);
	aux->traj_mode = AUX_TRAJ_DONE;
      }
}

void
aux_traj_clamp_start (struct aux_t *aux, int8_t speed, int16_t clampin_pwm)
{
    aux->traj_mode = AUX_TRAJ_CLAMP;
    aux->clampin_pwm = clampin_pwm;
    speed_control_set_speed (&aux->cs->speed, speed);
    control_state_set_mode (&aux->cs->state, CS_MODE_TRAJ_CONTROL,
			    CS_MODE_BLOCKING_DETECTION);
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
	    speed_control_set_speed (&aux->cs->speed, 0);
	    control_state_finished (&aux->cs->state);
	    aux->pos = aux->reset_pos;
	    aux->traj_mode = AUX_TRAJ_DONE;
	  }
	break;
      }
}

/** Start find zero mode. */
void
aux_traj_find_zero_start (struct aux_t *aux, int8_t speed, int16_t reset_pos)
{
    aux->traj_mode = AUX_TRAJ_FIND_ZERO_NOT;
    aux->reset_pos = reset_pos;
    speed_control_set_speed (&aux->cs->speed, speed);
    control_state_set_mode (&aux->cs->state, CS_MODE_TRAJ_CONTROL, 0);
}

/** Find limit mode. */
void
aux_traj_find_limit (struct aux_t *aux)
{
    switch (aux->traj_mode)
      {
      case AUX_TRAJ_FIND_LIMIT:
	/* If blocking, limit is found. */
	if (aux->cs->blocking_detection.blocked)
	  {
	    /* Disable all but traj control. */
	    control_state_set_mode (&aux->cs->state, CS_MODE_TRAJ_CONTROL,
				    CS_MODE_TRAJ_CONTROL - 1);
	    output_set (aux->cs->output, 0);
	    aux->traj_mode = AUX_TRAJ_FIND_LIMIT_WAIT;
	    aux->wait = 3 * 225;
	  }
	break;
      case AUX_TRAJ_FIND_LIMIT_WAIT:
	if (!--aux->wait)
	  {
	    control_state_set_mode (&aux->cs->state, CS_MODE_NONE, 0);
	    control_state_finished (&aux->cs->state);
	    aux->pos = aux->reset_pos;
	    aux->traj_mode = AUX_TRAJ_DONE;
	  }
	break;
      }
}

/** Start find limit mode. */
void
aux_traj_find_limit_start (struct aux_t *aux, int8_t speed, int16_t reset_pos)
{
    aux->traj_mode = AUX_TRAJ_FIND_LIMIT;
    speed_control_set_speed (&aux->cs->speed, speed);
    control_state_set_mode (&aux->cs->state, CS_MODE_TRAJ_CONTROL,
			    CS_MODE_BLOCKING_DETECTION);
    aux->reset_pos = reset_pos;
}

/** Update trajectories for one motor. */
static void
aux_traj_update_single (struct aux_t *aux)
{
    if (aux->cs->state.modes & CS_MODE_TRAJ_CONTROL)
      {
	switch (aux->traj_mode)
	  {
	  case AUX_TRAJ_SPEED:
	    aux_traj_speed (aux);
	    break;
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

