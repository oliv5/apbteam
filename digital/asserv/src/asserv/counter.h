#ifndef counter_h
#define counter_h
/* counter.h */
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

extern uint16_t counter_left, counter_right,
       counter_aux[AC_ASSERV_AUX_NB];
extern uint32_t counter_right_correction;
extern int16_t counter_left_diff, counter_right_diff,
       counter_aux_diff[AC_ASSERV_AUX_NB];

void
counter_init (void);

void
counter_update_step (void);

void
counter_update (void);

#endif /* counter_h */
