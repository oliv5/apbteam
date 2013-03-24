/* debug_avr.c */
/* Beacon debug interface. {{{
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
#include <appTimer.h>
#include "configuration.h"
#include "print.h"
#include "debug_avr.h"
#include "motor.h"
#include "reset.h"

HAL_UsartDescriptor_t appUsartDescriptor;          			// USART descriptor (required by stack)
static HAL_AppTimer_t debugTimer;						// TIMER descripor used by the DEBUG task

uint8_t usartRxBuffer[APP_USART_RX_BUFFER_SIZE];   	// USART Rx buffer
uint8_t usartTxBuffer[APP_USART_TX_BUFFER_SIZE];   	// USART Tx buffer

uint8_t debug_network = 0;
uint8_t debug_network_enable = 0;

/* This function initializes the USART interface for debugging on avr */
 void initSerialInterface(void)
 {
	appUsartDescriptor.tty             	= USART_CHANNEL;
	appUsartDescriptor.mode            	= USART_MODE_ASYNC;
	appUsartDescriptor.baudrate        	= USART_BAUDRATE_38400;
	appUsartDescriptor.dataLength     	= USART_DATA8;
	appUsartDescriptor.parity          	= USART_PARITY_EVEN;
	appUsartDescriptor.stopbits        	= USART_STOPBIT_1;
	appUsartDescriptor.rxBuffer        	= usartRxBuffer;
	appUsartDescriptor.rxBufferLength	= sizeof(usartRxBuffer);
	appUsartDescriptor.txBuffer        	= NULL; // use callback mode
	appUsartDescriptor.txBufferLength	= 0;
	appUsartDescriptor.rxCallback      	= usartRXCallback;
	appUsartDescriptor.txCallback      	= usartTXCallback;
	appUsartDescriptor.flowControl     = USART_FLOW_CONTROL_NONE;
	OPEN_USART(&appUsartDescriptor);
 }

/* RX USART Callback */
void usartRXCallback(uint16_t bytesToRead)
{
	uint8_t rxBuffer;	
	
	/* Read RX buffer from HAL */
	READ_USART(&appUsartDescriptor,&rxBuffer,bytesToRead);
	
	switch(rxBuffer)
	{
		case 'd':
			debug_start_stop_task();
			break;
		case 't':
			motor_start_stop_control();
			break;
		case 'w':
			motor_set_speed(motor_get_target_speed()+1);
			uprintf("target speed = %d\r\n",motor_get_target_speed());
			break;
		case 'x':
			motor_set_speed(motor_get_target_speed()-1);
			uprintf("target speed = %d\r\n",motor_get_target_speed());
			break;
		/* Default */
		default :
			uprintf(" ?? Unknown command ??\r\n");
			break;
	}
}

/* This function starts the debug task */
void debug_start_stop_task(void)
{
	static bool debug_task_running = 0;
	if(debug_task_running == 0)
	{
		debugTimer.interval = DEBUG_TASK_PERIOD;
		debugTimer.mode     = TIMER_REPEAT_MODE;
		debugTimer.callback = debug_task;
		HAL_StartAppTimer(&debugTimer);
		debug_task_running = 1;
	}
	else
	{
		HAL_StopAppTimer(&debugTimer);
		debug_task_running = 0;
	}
}

/* Debug task callback */
void debug_task(void)
{
 	uprintf("------------------------- debug TASK -------------------------\r\n");
	uprintf("## Motor\r\n");
	uprintf("State : %d   ----    speed = %d\r\n",motor_get_state(),OCR0A);
}
