/* debug_avr.h */
/* Macro for debug traces for avr target. {{{
 *
 * Copyright (C) 2011 Florent Duchon
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

#ifndef _DEBUG_AVR_H
#define _DEBUG_AVR_H

#include <stdio.h>
#include "configuration.h"

#define OPEN_USART            		HAL_OpenUsart
#define CLOSE_USART           	HAL_CloseUsart
#define WRITE_USART          		HAL_WriteUsart
#define READ_USART           		HAL_ReadUsart
#define USART_CHANNEL    		APP_USART_CHANNEL
#define DEBUG_TASK_PERIOD 	100L

typedef enum
{
	FREE,
	BUSY
} TUSART_bus_state;

typedef enum
{
	EMPTY,
	FILLED
} TUSART_buffer_level;

/* This function initializes the USART interface for debugging on avr */
void initSerialInterface(void);

/* TX USART Callback */
void usartTXCallback(void);

/* RX USART Callback */
void usartRXCallback(uint16_t bytesToRead);

/* This function sends data string via the USART interface */
void uprintf(char *format, ...);

/* This function starts the debug task */
void debug_start_stop_task(void);

/* Debug task callback */
void debug_task(void);

#endif
