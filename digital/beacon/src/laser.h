/* laser.h */
/* laser sensor management. {{{
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

#ifndef _LASER_H
#define _LASER_H

#define RISING_EDGE 			TCCR3B & (1<<ICES3)
#define SENDING_ENGAGED 		TIMSK3&(1<<OCIE3B)
#define LASER_CONFIRMATION_OFFSET 	10

typedef enum
{
	LASER_FIRST_RISING_EDGE,
	LASER_RISING_EDGE,
	LASER_FALLING_EDGE
} TLaser_edge_type;

typedef struct
{
	uint16_t angle;
} laser_s;

/* This function initializes the laser pin input and associated interrupt */
void laser_init(void);

/* This function returns the edge type trigged by the AVR IC module*/
TLaser_edge_type laser_get_edge_type(void);

/* This function inverts the trigged edge of the AVR IC module */
void laser_invert_IRQ_edge_trigger(void);

/* This function deactivates the angle sending */
void laser_inhibit_angle_confirmation(void);

/* This function configures the AVR OC3B interrupt that will send the angle LASER_SENDING_OFFSET after the latest rising edge */
void laser_engage_angle_confirmation(uint16_t value);

#endif
