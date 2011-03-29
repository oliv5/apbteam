#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h - motor/encoder/ext configuration template. */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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

/* motor/encoder/ext - External encoder module. */
/** Number of encoders. */
#define AC_ENCODER_EXT_NB 4
/** Use external memory hardware interface. */
#define AC_ENCODER_EXT_USE_XMEM 0
/** If not using XMEM, address/data bus, see io_bus.h. */
#define AC_ENCODER_EXT_AD_BUS A, 8, 0
/** If not using XMEM, address latch enable IO. */
#define AC_ENCODER_EXT_ALE_IO B, 0
/** If not using XMEM, read enable IO, valid low. */
#define AC_ENCODER_EXT_RD_IO B, 1
/** If not using XMEM and write available, write enable IO, valid low. */
#define AC_ENCODER_EXT_WR_IO B, 2
/** Reverse flag for each encoder (1 to reverse direction). */
#define AC_ENCODER_EXT_REVERSE 0, 0, 0, 0
/** Right shift for all encoders to lower resolution. */
#define AC_ENCODER_EXT_SHIFT 0
/** For debug purpose only! */
#define AC_ENCODER_EXT_EXPORT_READ 0

#endif /* avrconfig_h */
