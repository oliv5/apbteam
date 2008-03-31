#ifndef getsamples_h
#define getsamples_h
/* getsamples.h */
/*  {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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

/** getsamples FSM associated data. */
struct getsamples_data_t
{
    /* Distributor x position to get samples. */
    uint32_t distributor_x;
    /* Distributor y position to get samples. */
    uint32_t distributor_y;
    /* Samples to take. */
    uint8_t samples;
};

/** getsamples global. */
extern struct getsamples_data_t getsamples_data;

/** Start a getsamples FSM. */
void
getsamples_start (uint32_t distributor_x, uint32_t distributor_y,
		  uint8_t samples);

#endif /* getsamples_h */
