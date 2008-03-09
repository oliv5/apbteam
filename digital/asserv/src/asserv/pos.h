#ifndef pos_h
#define pos_h
/* pos.h */
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

extern uint32_t pos_theta_cur, pos_alpha_cur;
extern uint32_t pos_theta_cons, pos_alpha_cons;

extern int32_t pos_e_sat;
extern int32_t pos_int_sat;
extern uint16_t pos_theta_kp, pos_alpha_kp;
extern uint16_t pos_theta_ki, pos_alpha_ki;
extern uint16_t pos_theta_kd, pos_alpha_kd;
extern int32_t pos_blocked;

extern int32_t pos_theta_int, pos_alpha_int;
extern int32_t pos_theta_e_old, pos_alpha_e_old;

void
pos_update (void);

void
pos_reset (void);

#endif /* pos_h */
