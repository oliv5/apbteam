/* twi_specific.h */
/* twi specific funtion. {{{
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

#ifndef _TWI_SPECIFIC_H
#define _TWI_SPECIFIC_H

#define TWI_TASK_PERIOD 	4L


typedef enum
{
	TWI_RX_CRC_FIELD,
	TWI_RX_SEQ_FIELD,
	TWI_RX_JACK_FIELD,
	TWI_RX_COLOR_FIELD,
	TWI_RX_NB_ADV_FIELD,
	TWI_RX_X_MSB_FIELD,
	TWI_RX_X_LSB_FIELD,
	TWI_RX_Y_MSB_FIELD,
	TWI_RX_Y_LSB_FIELD
} TTWI_RX_field;

typedef enum
{
	TWI_TX_CRC_FIELD,
	TWI_TX_SEQ_FIELD,
	TWI_TX_X1_MSB_FIELD,
	TWI_TX_X1_LSB_FIELD,
	TWI_TX_Y1_MSB_FIELD,
	TWI_TX_Y1_LSB_FIELD,
	TWI_TX_TRUST1_FIELD,
	TWI_TX_X2_MSB_FIELD,
	TWI_TX_X2_LSB_FIELD,
	TWI_TX_Y2_MSB_FIELD,
	TWI_TX_Y2_LSB_FIELD,
	TWI_TX_TRUST2_FIELD,
	TWI_TX_FIELD_NB
} TTWI_TX_field;

/* This function manages the TWI RX/RX transferts */
void twi_task(void);

/* Initialisze specific TWI */
void twi_init_specific(void);

#endif
