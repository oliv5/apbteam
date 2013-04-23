/* print.c */
/* Zigbit common functions for debug {{{
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

#include <stdarg.h>
#include <string.h>
#include <usart.h>
#include "configuration.h"
#include "print.h"
#include "uid.h"


extern HAL_UsartDescriptor_t appUsartDescriptor;          			// USART descriptor (required by stack)
extern uint8_t usartTxBuffer[APP_USART_TX_BUFFER_SIZE];   	// USART Tx buffer


static TUSART_buffer_level TXbuffer_level = EMPTY;		// TX buffer state
static TUSART_bus_state TXbus_state = FREE;				// TX line state

static uint16_t start_offset = 0;							// Start offset for TX buffer
static uint16_t end_offset = 0;								// Stop offset for TX buffer


/* TX USART Callback */
void usartTXCallback(void)
{
	/* If buffer is not empty continue to send via USART line */ 
	if(TXbuffer_level != EMPTY)
	{
		WRITE_USART(&appUsartDescriptor,usartTxBuffer+start_offset,end_offset);
		TXbuffer_level = EMPTY;
	}
	else
	{
		/* Bus is free so reset variables and flags */
		memset(usartTxBuffer,0,APP_USART_TX_BUFFER_SIZE);
		start_offset = 0;
		end_offset = 0;
		TXbus_state = FREE;
	}
}

/* This function sends data string via the USART interface */
void uprintf(char *format, ...)
{
	va_list args;
	va_start(args,format);
	
	/* UART security for coordinator */
	if(get_uid() == 0)
		return;
	
	if(end_offset+strlen(format)+sizeof(args) < APP_USART_TX_BUFFER_SIZE)
	{
		vsprintf(usartTxBuffer+end_offset,format,args);
		end_offset = strlen(usartTxBuffer);
		
		/* Check if the bus is busy */
		if(TXbus_state == FREE)
		{
			WRITE_USART(&appUsartDescriptor,usartTxBuffer+start_offset,end_offset);
			start_offset = end_offset;
			TXbus_state = BUSY;
		}
		else
		{
			TXbuffer_level = FILLED;
		}
	}
	va_end(args);
}
