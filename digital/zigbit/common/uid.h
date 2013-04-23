/* uid.h */
/* Zigbit common UID function {{{
 *
 * Copyright (C) 2012 Florent Duchon
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

#ifndef _UID_H
#define _UID_H

#define APBTEAM_MAX_ZIGBIT_DEVICES 16

typedef enum 
{
	ZIGBIT_NOT_DEFINED,
	ZIGBIT_BEACON,
	ZIGBIT_DONGLE,
} TFunctionnality;

typedef struct 
{
	uint16_t uid;
	DeviceType_t type;
	TFunctionnality func;
} zigbit_definition_s;

/* This function returns the UID of the device according to the mapping of F5 F6 F7 F8*/
uint16_t get_uid(void);

/* This function returns the device type */
DeviceType_t get_device_type(uint16_t uid);

/* This function returns the device functionnaly */
TFunctionnality get_device_functionnality(uint16_t uid);

#endif
