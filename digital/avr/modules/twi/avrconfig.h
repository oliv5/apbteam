#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h - TWI module configuration template. */
/* avr.twi - TWI AVR module. {{{
 *
 * Copyright (C) 2005 Demonchy Clément
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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

/* twi - TWI module. */
/** Activate master part. */
#define AC_TWI_MASTER_ENABLE 0
/** Activate slave part. */
#define AC_TWI_SLAVE_ENABLE 0
/** Slave recv buffer size. */
#define AC_TWI_SL_RECV_BUFFER_SIZE 16
/** Slave send buffer size. */
#define AC_TWI_SL_SEND_BUFFER_SIZE 16

#endif /* avrconfig_h */
