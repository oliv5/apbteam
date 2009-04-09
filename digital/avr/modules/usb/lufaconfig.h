#ifndef lufaconfig_h
#define lufaconfig_h
/* lufaconfig.h */
/* usb - USB device module using LUFA. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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

#define F_CPU AC_FREQ
#define USE_NONSTANDARD_DESCRIPTOR_NAMES
#define NO_STREAM_CALLBACKS
#define USB_DEVICE_ONLY
#define USE_STATIC_OPTIONS (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED \
			    | USB_OPT_AUTO_PLL)

#endif /* lufaconfig_h */
