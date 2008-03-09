/* motor_model.c - DC motor model. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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
 * Switching to french for all thoses non english speaking people.
 *
 * Ce fichier permet de modéliser un moteur à courant continue. Il y a deux
 * parties, la modélisation électrique et la modélisation mécanique.
 *
 * On peut trouver de l'aide sur :
 *  - le site de maxon : http://www.maxonmotor.com/
 *  - ici : http://www.mathworks.com/access/helpdesk/help/toolbox/control/getstart/buildmo4.html
 *  - là : http://iai.eivd.ch/users/mee/Regulation_automatique_anal.htm
 *
 * Voici ce qui en résulte, des belles équations différentielles :
 * u(t) = R i(t) + L di(t)/dt + 1/Ke o(t)
 * J do(t)/dt = Kt i(t) - Rf o(t)
 * dth(t)/dt = o(t)
 *
 * Les variables sont décrites dans la structure motor_t.
 *
 * À cela, il faut ajouter un coef pour le réducteur, je vous laisse ça en
 * exercice.
 *
 * On va résoudre ces belles équadiff numériquement par la méthode d'Euler (il
 * est partout celui là). Si vous voulez plus de détail, mailez moi. On arrive
 * à :
 *
 * i(t+h) = i(t) + h (1/L u(t) - R/L i(t) - 1/(Ke L) o(t))
 * o(t+h) = o(t) + h (i_G^2 ro_G)/J (Kt i(t) - Rf o(t))
 * th(t+h) = th(t) + h o(t)
 *
 * C'est consternant de simplicité non ?
 */

/** Make a simulation step. */
void motor_model_step (struct motor_t *m)
{
    double i_, o_, th_; /* New computed values. */
    double h; /* Infinitesimal step...  Well, not so infinite. */
    int d;
    d = m->d;
    h = m->h / d;
    /* Make several small steps to increase precision. */
    for (; d; d--)
      {
	/* Ah, the mistical power of computation. */
	i_ = m->i
	    + h * (1.0 / m->L * m->u
		   - m->R / m->L * m->i
		   - 1.0 / m->Ke / m->L * m->o);
	o_ = m->o
	    + h * m->i_G * m->i_G * m->ro_G / m->J
	    * (m->Kt * m->i - m->Rf * m->o);
	th_ = m->th + h * m->o;
	/* Ok, now store this step. */
	m->i = i_;
	m->o = o_;
	m->th = th_;
      }
    /* Damn!  It's finished yet! */
    m->t += m->h;
}

