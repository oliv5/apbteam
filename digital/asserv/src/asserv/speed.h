#ifndef speed_h
#define speed_h
/* speed.h */
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

extern int16_t speed_theta_cur, speed_alpha_cur;
extern int16_t speed_theta_cons, speed_alpha_cons;
extern int8_t speed_theta_max, speed_alpha_max;
extern int8_t speed_theta_slow, speed_alpha_slow;
extern uint32_t speed_theta_pos_cons, speed_alpha_pos_cons;
extern uint8_t speed_pos;

extern int16_t speed_theta_acc, speed_alpha_acc;

void
speed_update (void);

#endif /* speed_h */
