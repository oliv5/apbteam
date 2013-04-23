/* uid.c */
/* Zigbit common UID function {{{
 *
 * Copyright (C) 2013 Florent Duchon
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

/* This function returns the UID of the device according to the mapping of F5 F6 F7 F8*/

#include "uid.h"
#include <util/delay.h>

/* APBTeam static zigbit device list */
static zigbit_definition_s apbeam_zigbit_table[APBTEAM_MAX_ZIGBIT_DEVICES] =
{
	{0	,DEVICE_TYPE_COORDINATOR	,ZIGBIT_BEACON},
	{1	,DEVICE_TYPE_ROUTER	,ZIGBIT_BEACON},
	{2	,DEVICE_TYPE_ROUTER	,ZIGBIT_BEACON},
	{3	,DEVICE_TYPE_ROUTER	,ZIGBIT_BEACON},
	{4	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{5	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{6	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{7	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{8	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{9	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{10	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{11	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{12	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{13	,DEVICE_TYPE_ROUTER	,ZIGBIT_NOT_DEFINED},
	{14	,DEVICE_TYPE_ROUTER	,ZIGBIT_DONGLE},
	{15	,DEVICE_TYPE_ROUTER	,ZIGBIT_DONGLE},
};

/* This function returns the UID of the device according to the mapping of F5 F6 F7 F8*/
uint16_t get_uid(void)
{
	uint16_t value = 0x0F;
	
	/* activated pull up on F4 -> F7 */ 
  	PORTF = 0xF0;

	/* F4 -> F7 configured as input */ 
 	DDRF = 0x00;
	
	_delay_ms(10);
	
	if(PINF&0x20) //PINF5
		value &= 0xF7;
	
	if(PINF&0x80) //PINF7
		value &= 0xFB;
	
	if(PINF&0x40) //PINF6
		value &= 0xFD;
	
	if(PINF&0x10) //PINF4
		value &= 0xFE;
	
	/* Nothing connected */
// 	if(PINF&0xFF)
// 		value &=0xF0;

	return value;
}

/* This function returns the device type */
DeviceType_t get_device_type(uint16_t uid)
{
	return apbeam_zigbit_table[uid].type;
}

/* This function returns the device functionnaly */
TFunctionnality get_device_functionnality(uint16_t uid)
{
	return apbeam_zigbit_table[uid].func;
}
