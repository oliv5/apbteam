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
#include "calibration.h"
#include "debug_avr.h"
#include "servo.h"
#include "codewheel.h"
#include "laser.h"
#include "network.h"
#include "motor.h"
#include "misc.h"
#include "position.h"

static HAL_UsartDescriptor_t appUsartDescriptor;          			// USART descriptor (required by stack)
static HAL_AppTimer_t debugTimer;						// TIMER descripor used by the DEBUG task

static uint8_t usartRxBuffer[APP_USART_RX_BUFFER_SIZE];   	// USART Rx buffer
static uint8_t usartTxBuffer[APP_USART_TX_BUFFER_SIZE];   	// USART Tx buffer

static TUSART_buffer_level TXbuffer_level = EMPTY;		// TX buffer state
static TUSART_bus_state TXbus_state = FREE;				// TX line state

static uint16_t start_offset = 0;							// Start offset for TX buffer
static uint16_t end_offset = 0;								// Stop offset for TX buffer

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
			calibration_set_laser_flag(SET_SERVO_1);
			break;
		case 's':
			calibration_set_laser_flag(SET_SERVO_2);
			break;
		case 'r':
			reset_avr();
			break;
		case 't':
			motor_start_stop_control();
			break;
		case 'j':
			jack_on_off();
			break;
		case '0':
			network_send_data(NETWORK_RESET,0x1);
			break;
		case 'w':
			motor_set_target_speed(motor_get_target_speed()-1);
			uprintf("target speed = %d\r\n",motor_get_target_speed());
			break;
		case 'x':
			motor_set_target_speed(motor_get_target_speed()+1);
			uprintf("target speed = %d\r\n",motor_get_target_speed());
			break;
		case 'f':
			if(debug_network_enable == 1)
			{
				debug_network_enable = 0;
				debug_network = 0;
			}
			else
			{
				debug_network_enable = 1;
				debug_network = 1;
			}
			break;
		/* Default */
		default :
			uprintf(" ?? Unknown command ??\r\n");
			break;
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

/* Debug task callback */
void debug_task(void)
{
 	uprintf("------------------------- debug TASK -------------------------\r\n");
	uprintf("## Network\r\n");
	uprintf("Status : 0x%x - ",network_get_status());
	uprintf("LQI = %d - ",network_get_lqi());
	uprintf("RSSI = %d - \r\n",network_get_rssi());
#ifdef TYPE_COOR
	uprintf("X = %d   ---   ",position_get_coord(OPPONENT_1,X));
	uprintf("Y = %d   ---   ",position_get_coord(OPPONENT_1,Y));
	uprintf("Trust = %d\r\n",position_get_trust(OPPONENT_1));
#else
	uprintf("## Calibration\r\n");
	uprintf("State : %d\r\n",calibration_get_state());
	uprintf("## Servo\r\n");
 	uprintf("State : [1]=%d [2]=%d - ",servo_get_state(SERVO_1),servo_get_state(SERVO_2));
 	uprintf("Value : [1]=%d [2]=%d\r\n",servo_get_value(SERVO_1),servo_get_value(SERVO_2));
	uprintf("## Codewheel\r\n");
	uprintf("State = %d - Raw = %d - Degree = %f\r\n",codewheel_get_state(),codewheel_get_value(),codewheel_convert_angle_raw2degrees(codewheel_get_value()));
	uprintf("## Motor\r\n");
	uprintf("State : %d   ----    speed = %d\r\n",motor_get_state(),OCR0A);
#endif
}
