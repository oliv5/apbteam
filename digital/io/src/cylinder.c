/* cylinder.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Haller
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
#include "cylinder.h"

/* last state of optical fence */
uint8_t cylinder_of_state = 0;

/* nb puck in cylinder */
uint8_t cylinder_nb_puck = 0;

uint8_t cylinder_puck_on_of = 0;

/* force cylinder to be closed */
uint8_t cylinder_close_order = 0;

/* flush puck on the cylinder */
uint8_t cylinder_flush_order = 0;

/* distributor fucked */
uint8_t cylinder_distributor_fucked = 0;

/* distributor empty */
uint8_t cylinder_distributor_empty = 0;

/* cylinder order */
