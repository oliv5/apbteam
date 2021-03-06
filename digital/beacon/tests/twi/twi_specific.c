/* twi_specific.c */
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

#include "modules/utils/byte.h"
#include "modules/utils/crc.h"
#include "twi_specific.h"
#include "configuration.h"
#include "twi.h"
#include "network.h"


static HAL_AppTimer_t twiTimer;

uint16_t opponent1_x = 13;
uint16_t opponent1_y = 37;
uint16_t opponent1_trust = 42;
uint16_t opponent2_x = 1500;
uint16_t opponent2_y = 1671;
uint16_t opponent2_trust = 99;

/* This function manages the TWI RX/RX transferts */
void twi_task(void)
{
	uint8_t TXbuffer[AC_TWI_SLAVE_SEND_BUFFER_SIZE];
	uint8_t RXbuffer[AC_TWI_SLAVE_RECV_BUFFER_SIZE];	
	uint8_t RXlen;
	static uint8_t seq = 0;
	
	TXbuffer[TWI_TX_SEQ_FIELD] = seq;

	/* Opponent 1 */
	TXbuffer[TWI_TX_X1_MSB_FIELD] 	= v16_to_v8 (opponent1_x, 1);
	TXbuffer[TWI_TX_X1_LSB_FIELD] 	= v16_to_v8 (opponent1_x, 0);
	TXbuffer[TWI_TX_Y1_MSB_FIELD] 	= v16_to_v8 (opponent1_y, 1);
	TXbuffer[TWI_TX_Y1_LSB_FIELD] 	= v16_to_v8 (opponent1_y, 0);
	TXbuffer[TWI_TX_TRUST1_FIELD] 	= opponent1_trust;

	/* Opponent 2 */
	TXbuffer[TWI_TX_X2_MSB_FIELD] 	= v16_to_v8 (opponent2_x, 1);
	TXbuffer[TWI_TX_X2_LSB_FIELD] 	= v16_to_v8 (opponent2_x, 0);
	TXbuffer[TWI_TX_Y2_MSB_FIELD] 	= v16_to_v8 (opponent2_y, 1);
	TXbuffer[TWI_TX_Y2_LSB_FIELD] 	= v16_to_v8 (opponent2_y, 0);
	TXbuffer[TWI_TX_TRUST2_FIELD] 	= opponent2_trust;
	
	/* Compute CRC */
	TXbuffer[TWI_TX_CRC_FIELD] 	= crc_compute (&TXbuffer[1],TWI_TX_FIELD_NB-1);
	
	/* data to be communicated to the master */
	twi_slave_update (TXbuffer, sizeof (TXbuffer));
	
	/* Check for data. */
	RXlen = twi_slave_poll (RXbuffer, AC_TWI_SLAVE_RECV_BUFFER_SIZE);
	
	/* data availlable */
	if(RXlen != 0)
	{
		if (crc_compute (RXbuffer + 1, RXlen - 1) == RXbuffer[0])
		{
			if(RXbuffer[TWI_RX_JACK_FIELD] == 1)
			{
				PORTD=0xFF;
			}
			else
			{
				PORTD=0x00;
			}
		}
		else
		{
			/* CRC not valid, dump values */
		}
	}
}

/* Initialisze specific TWI */
void twi_init_specific(void)
{
	twi_init(AC_BEACON_TWI_ADDRESS);
	twiTimer.interval = TWI_TASK_PERIOD;
	twiTimer.mode     = TIMER_REPEAT_MODE;
	twiTimer.callback = twi_task;
	HAL_StartAppTimer(&twiTimer);
}

