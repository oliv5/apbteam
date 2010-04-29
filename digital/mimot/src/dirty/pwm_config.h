#ifndef pwm_config_h
#define pwm_config_h
/* pwm_config.h - Helper for PWM configuration. */
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

/* Simplify conditionnal compilation. */
#define PWM1or2 (defined (PWM1) || defined (PWM2))
#ifdef PWM1
# define PWM1c(x) x
#else
# define PWM1c(x) 0
#endif
#ifdef PWM2
# define PWM2c(x) x
#else
# define PWM2c(x) 0
#endif

#endif /* pwm_config_h */
