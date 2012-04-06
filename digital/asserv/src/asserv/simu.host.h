#ifndef simu_host_h
#define simu_host_h
/* simu.host.h */
/* asserv - Position & speed motor control on AVR. {{{
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
#include "models.host.h"

extern uint8_t DDRF, PORTB, PORTC, PORTD, PORTE, PORTF, PORTG, PINC;

extern double simu_pos_x, simu_pos_y, simu_pos_a;

extern motor_model_t simu_left_model, simu_right_model,
       simu_aux_model[AC_ASSERV_AUX_NB];

extern const struct robot_t *simu_robot;

void
timer_init (void);

void
timer_wait (void);

uint8_t
timer_read (void);

void
simu_compute_absolute_position (double p_x, double p_y, double *x, double *y);

#endif /* simu_host_h */
