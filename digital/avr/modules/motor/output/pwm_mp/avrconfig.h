#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h - motor/output/pwm_mp configuration template. */
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

/* motor/output/pwm_mp - Motor Power board PWM output module. */
/** Number of outputs, there is two outputs per board. */
#define AC_OUTPUT_PWM_MP_NB 4
/** Slave select for first Motor Power board.
 * WARNING: this must match hardware SS pin if using hardware SPI! */
#define AC_OUTPUT_PWM_MP_SPI_SS_IO_0 B, 0
/** Slave select for next Motor Power boards. */
#define AC_OUTPUT_PWM_MP_SPI_SS_IO_1 E, 4

#endif /* avrconfig_h */
