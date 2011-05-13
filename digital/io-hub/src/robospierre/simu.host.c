/* simu.host.c - Host simulation. */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "simu.host.h"

#include "modules/utils/utils.h"
#include "modules/host/host.h"
#include "modules/host/mex.h"
#include "io.h"

/** AVR registers. */
uint8_t PINA, PINE, PINF;

/** Initialise simulation. */
void
simu_init (void)
{
    const char *mex_instance;
    mex_node_connect ();
    mex_instance = host_get_instance ("io-hub0", 0);
}

/** Make a simulation step. */
void
simu_step (void)
{
}

void
timer_init (void)
{
    simu_init ();
}

uint8_t
timer_wait (void)
{
    mex_node_wait_date (mex_node_date () + 4);
    simu_step ();
    return 0;
}

