#ifndef motor_model_defs_host_h
#define motor_model_defs_host_h
/* motor_model_defs.host.h */
/* motor - Motor control module. {{{
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

#include "motor_model.host.h"

extern const motor_model_def_t motor_model_def_re25cll_x10;
extern const motor_model_def_t motor_model_def_re25g_x20_25;
extern const motor_model_def_t motor_model_def_amax32ghp_x16;
extern const motor_model_def_t motor_model_def_faulhaber_2657_x9_7;
extern const motor_model_def_t motor_model_def_faulhaber_2342_x3_71;

/** Get a motor model definition by its name. */
const motor_model_def_t *
motor_model_def_get (const char *name);

#endif /* motor_model_defs_host_h */
