/* print.h */
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

#ifndef _PRINT_H
#define _PRINT_H

#include <stdio.h>
#include <usart.h>
#include "configuration.h"

#define OPEN_USART            		HAL_OpenUsart
#define CLOSE_USART           	HAL_CloseUsart
#define WRITE_USART          		HAL_WriteUsart
#define READ_USART           		HAL_ReadUsart
#define USART_CHANNEL    		APP_USART_CHANNEL

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

/* TX USART Callback */
void usartTXCallback(void);

/* This function sends data string via the USART interface */
void uprintf(char *format, ...);

#endif