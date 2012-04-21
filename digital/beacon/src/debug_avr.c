/* debug.c */
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
#include "calibration.h"
#include "debug_avr.h"
#include "servo.h"
#include "codewheel.h"
#include "laser.h"
#include "network.h"

HAL_UsartDescriptor_t appUsartDescriptor;          			// USART descriptor (required by stack)
HAL_AppTimer_t debugTimer;						// TIMER descripor used by the DEBUG task
HAL_AppTimer_t wheelTimer;						// TIMER descripor used by the DEBUG task

uint8_t usartRxBuffer[APP_USART_RX_BUFFER_SIZE];   	// USART Rx buffer
uint8_t usartTxBuffer[APP_USART_TX_BUFFER_SIZE];   	// USART Tx buffer

TUSART_buffer_level TXbuffer_level = EMPTY;		// TX buffer state
TUSART_bus_state TXbus_state = FREE;				// TX line state

uint16_t start_offset = 0;							// Start offset for TX buffer
uint16_t end_offset = 0;								// Stop offset for TX buffer



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

/* TX USART Callback */
void usartTXCallback(void)
{
	/* If buffer is not empty continue to send via USART line */ 
	if(TXbuffer_level != EMPTY)
	{
		WRITE_USART(&appUsartDescriptor,usartTxBuffer+start_offset,end_offset);
		TXbuffer_level = FILLED;
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

/* RX USART Callback */
void usartRXCallback(uint16_t bytesToRead)
{
	uint8_t rxBuffer;	
	
	/* Read RX buffer from HAL */
	READ_USART(&appUsartDescriptor,&rxBuffer,bytesToRead);
	
	switch(rxBuffer)
	{
		case 'o':
			/* Increase servo 1 angle */
			uprintf("SERVO_1 = %d\r\n",servo_angle_increase(SERVO_1));
			break;
		case 'l':
			/* Decrease servo 1 angle */
			uprintf("SERVO_1 = %d\r\n",servo_angle_decrease(SERVO_1));
			break;
		case 'p':
			/* Increase servo 2 angle */
			uprintf("SERVO_2 = %d\r\n",servo_angle_increase(SERVO_2));
			break;
		case 'm':
			/* Decrease servo 2 angle */
			uprintf("SERVO_2 = %d\r\n",servo_angle_decrease(SERVO_2));
			break;
		case 'a':
			uprintf("CodeWheel Value = %d\r\n",codewheel_get_value());
			break;
		case 'd':
			debug_start_stop_task();
			break;
		case 'z':
			codewheel_reset();
			break;
		case 'c':
			calibration_start_stop_task();
			break;
		case 'q':
			calibration_set_laser_flag(SET);
			break;
		case 'w':
// 			wheel_start_stop_task();
// 			uprintf("TCNT3 = %d\r\n",TCNT3);
			TIMSK3 &= ~(1<<OCIE3B);
		case 'r':
			led_off(1);
			led_off(2);
			led_off(3);
			break;
		/* Default */
		default :
			uprintf(" ?? Unknown command ??\r\n");
	}
}

/* This function sends data string via the USART interface */
void uprintf(char *format, ...)
{
	va_list args;
	va_start(args,format);
	
	if(end_offset+strlen(format)+strlen(args) < APP_USART_TX_BUFFER_SIZE)
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
void wheel_task(void)
{
	TCNT3++;
}

void wheel_start_stop_task(void)
{
	static bool wheel_task_running = 0;
	if(wheel_task_running == 0)
	{
		led_on(2);
		wheelTimer.interval = WHEEL_TASK_PERIOD;
		wheelTimer.mode     = TIMER_REPEAT_MODE;
		wheelTimer.callback = wheel_task;
		HAL_StartAppTimer(&wheelTimer);
		wheel_task_running = 1;
	}
	else
	{
		led_off(2);
		HAL_StopAppTimer(&wheelTimer);
		TCNT3=0;
		wheel_task_running = 0;
	}
}

/* Debug task callback */
void debug_task(void)
{
 	uprintf("------------------------- debug TASK -------------------------\r\n");
	uprintf("NWK : status = 0x%x\r\n",network_get_status());
#ifdef TYPE_END
 	uprintf("[1] Scanning State = %d -- Value = %d\r\n",servo_get_state(SERVO_1),servo_get_value(SERVO_1));
 	uprintf("[2] Scanning State = %d -- Value = %d\r\n",servo_get_state(SERVO_2),servo_get_value(SERVO_2));
	uprintf("CodeWheel = %d\r\n",codewheel_get_value());
	uprintf("Calibration state = %d\r\n",calibration_get_state());
#endif
}
