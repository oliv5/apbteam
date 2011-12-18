/* motor_model.c - DC motor model. */
/* motor - Motor control module. {{{
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
#include "motor_model.host.h"

/**
 * This file models a DC motor.  There is two parts, electric model and
 * mechanic model.
 *
 * You can find documentation about this subject on:
 *  - maxon motor web site: http://www.maxonmotor.com/
 *
 * From there, you can find the following differential equations:
 * u(t) = R i(t) + L di(t)/dt + 1/Ke o(t)
 * J do(t)/dt = Kt i(t) - Rf o(t)
 * dth(t)/dt = o(t)
 *
 * (see motor_t for variables description)
 *
 * The gearbox reduction and efficiency have to be added in the equations,
 * this is left as an exercice for the reader.
 *
 * Those equations can be solved numerically thanks to Euler method:
 *
 * i(t+h) = i(t) + h (1/L u(t) - R/L i(t) - 1/(Ke L) o(t))
 * o(t+h) = o(t) + h (i_G^2 ro_G)/J (Kt i(t) - Rf o(t))
 * th(t+h) = th(t) + h o(t)
 */

/** Make a simulation step. */
void motor_model_step (motor_model_t *m)
{
    double i_, o_, th_; /* New computed values. */
    double h; /* Infinitesimal step...  Well, not so infinite. */
    int d;
    d = m->d;
    h = m->h / d;
    /* Make several small steps to increase precision. */
    for (; d; d--)
      {
	/* Make one little step. */
	i_ = m->i
	    + h * (1.0 / m->m.L * m->u
		   - m->m.R / m->m.L * m->i
		   - 1.0 / m->m.Ke / m->m.L * m->o);
	o_ = m->o
	    + h * m->m.i_G * m->m.i_G * m->m.ro_G / m->m.J
	    * (m->m.Kt * m->i - m->m.Rf * m->o);
	th_ = m->th + h * m->o;
	/* Test for limits overflow, I have no proof it works right for the
	 * moment, only suspicions. */
	if (th_ < m->m.th_min)
	  {
	    th_ = m->m.th_min;
	    o_ = 0.0;
	  }
	else if (th_ > m->m.th_max)
	  {
	    th_ = m->m.th_max;
	    o_ = 0.0;
	  }
	/* OK, now store this step. */
	m->i = i_;
	m->o = o_;
	m->th = th_;
      }
    /* Finished! */
    m->t += m->h;
}

