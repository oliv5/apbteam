#ifndef cs_h
#define cs_h
/* cs.h - Control system definition. */
/* asserv - Position & speed motor control on AVR. {{{
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
#include "modules/motor/control_system/control_system.h"
#include "modules/motor/encoder/encoder_corrector.h"

extern encoder_t encoder_left, encoder_right;
extern encoder_corrector_t encoder_right_corrector;
extern output_t output_left, output_right;
extern control_system_polar_t cs_main;

#if AC_ASSERV_AUX_NB
extern encoder_t encoder_aux[AC_ASSERV_AUX_NB];
extern output_t output_aux[AC_ASSERV_AUX_NB];
extern control_system_single_t cs_aux[AC_ASSERV_AUX_NB];
#endif

void
cs_init (void);

void
cs_update_prepare (void);

void
cs_update (void);

void
cs_reset (void);

#endif /* cs_h */
