/* simu.host.c - Host simulation. */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "output.h"
#include "simu.host.h"

#include "modules/utils/utils.h"
#include "modules/host/host.h"
#include "modules/host/mex.h"
#include "modules/adc/adc.h"
#include "modules/path/path.h"
#include "io.h"

/** AVR registers. */
uint8_t PORTA, PORTC, PORTF, DDRA, DDRC, DDRF, PINA, PINE, PINF;

/** Message types. */
uint8_t simu_mex_pos_report;
uint8_t simu_mex_path;

static void
simu_adc_handle (void *user, mex_msg_t *msg)
{
    uint8_t index;
    uint16_t value;
    mex_msg_pop (msg, "BH", &index, &value);
    adc_values[index] = value;
}

/** Initialise simulation. */
void
simu_init (void)
{
    const char *mex_instance;
    mex_node_connect ();
    mex_instance = host_get_instance ("io-hub0", 0);
    uint8_t mtype = mex_node_reservef ("%s:adc", mex_instance);
    mex_node_register (mtype, simu_adc_handle, 0);
    simu_mex_pos_report = mex_node_reservef ("%s:pos-report", mex_instance);
    simu_mex_path = mex_node_reservef ("%s:path", mex_instance);
    output_host_init ();
}

/** Make a simulation step. */
void
simu_step (void)
{
    output_host_update ();
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

/** Send computed path. */
void
simu_send_path (vect_t *points, uint8_t len,
		struct path_obstacle_t *obstacles, uint8_t obstacles_nb)
{
    int i;
    mex_msg_t *m;
    m = mex_msg_new (simu_mex_path);
    for (i = 0; i < len; i++)
	mex_msg_push (m, "hh", points[i].x, points[i].y);
    mex_node_send (m);
}

void
simu_send_pos_report (vect_t *pos, uint8_t pos_nb, uint8_t id)
{
    mex_msg_t *m;
    m = mex_msg_new (simu_mex_pos_report);
    mex_msg_push (m, "b", id);
    for (; pos_nb; pos++, pos_nb--)
	mex_msg_push (m, "hh", pos->x, pos->y);
    mex_node_send (m);
}

