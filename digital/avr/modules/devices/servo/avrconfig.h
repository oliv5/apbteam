#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h */
/* avr.devices.servo - Servo AVR module. {{{
 *
 * Copyright (C) 2011 Maxime Hadjinlian
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

/* servo - Servo module. */
/** All servos are currently connected to the same port. */
#define AC_SERVO_PORT PORTA
#define AC_SERVO_DDR DDRA

#endif /* avrconfig_h */
