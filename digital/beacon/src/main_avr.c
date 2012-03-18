/**************************************************************************//**
\file Peer2Peer.c

\brief Peer-2-peer sample application.

\author
Atmel Corporation: http://www.atmel.com \n
Support email: avr@atmel.com

Copyright (c) 2008 , Atmel Corporation. All rights reserved.
Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
History:
14.10.09 A. Taradov - Added FIFO for received packets
******************************************************************************/

/******************************************************************************
		Includes section
******************************************************************************/
#include <types.h>
#include <configServer.h>
#include <appTimer.h>
#include <zdo.h>
#include "configuration.h"
#include "network.h"
// #include <serialInterface.h>
#include <irq.h>
#include "sensors.h"
#include "servo.h"
#include <stdio.h>
#include "debug.h"
#include "position.h"
#include <math.h>
#include <util/delay.h>
/******************************************************************************
		Define(s) section
******************************************************************************/

/******************************************************************************
		Variables section
******************************************************************************/

// Application related parameters
AppState_t appState = APP_INITIAL_STATE;  // application state
// int jack = 0;
// status_s status;
// extern int lost_packet;
// extern unsigned int angle;
/***********************************************************************************
		Static functions declarations section
***********************************************************************************/
// extern APS_DataReq_t test;
// extern buff_t buf_to_send;

/***********************************************************************************
		Implementation section
***********************************************************************************/
/**************************************************************************//**
\brief Application task handler.

\param  none.
\return none.
******************************************************************************/
#ifdef TYPE_COOR
	DeviceType_t deviceType = DEVICE_TYPE_COORDINATOR;
#else
 	DeviceType_t deviceType = DEVICE_TYPE_END_DEVICE;
#endif
int top = 0;
void APL_TaskHandler(void)
{
	switch (appState)
	{
		case APP_INITIAL_STATE:                 // Node has initial state
			
			/*D5/D6/D7 are configured in output */
			DDRD = 0xE0;
			/* Power on the led */
			int temp=0;
			for(temp=0;temp<3;temp++)
			{
				PORTD=0xE0;
				_delay_ms(100);
				PORTD=0;
				_delay_ms(100);
			}
			
//  			initSerialInterface();          // Open USART

			switch(deviceType)
			{
				case DEVICE_TYPE_COORDINATOR:
// 					error = init_twi();
// 					initNetwork();
					break;
				case DEVICE_TYPE_END_DEVICE:
// 					init_ILS();
// 					control_motor_ms(100);
// 					init_timer_servo();
// 					initNetwork();
					break;
				default:
					break;
			}
			appState = APP_NETWORK_JOINING_STATE;
			SYS_PostTask(APL_TASK_ID);      // Execute next step
			break;

		case APP_NETWORK_JOINING_STATE:
//    			startNetwork();
			break;
			
		case APP_NETWORK_LEAVING_STATE:
			break;
		case APP_NETWORK_JOINED_STATE:
// 			if(jack == 1)
// 			{
// 				send_angle(angle);
// 			}
			break;
		default:
			break;
	}
}



/**************************************************************************//**
\brief Wakeup event handler (dummy).

\param  none.
\return none.
******************************************************************************/
void ZDO_WakeUpInd(void)
{
}


#ifdef _BINDING_
/***********************************************************************************
Stub for ZDO Binding Indication

Parameters:
bindInd - indication

Return:
none

***********************************************************************************/
void ZDO_BindIndication(ZDO_BindInd_t *bindInd) 
{
(void)bindInd;
}

/***********************************************************************************
Stub for ZDO Unbinding Indication

Parameters:
unbindInd - indication

Return:
none

***********************************************************************************/
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
(void)unbindInd;
}
#endif //_BINDING_

/**********************************************************************//**
\brief Main - C program main start function

\param none
\return none
**************************************************************************/
int main(void)
{ 
//    	init_timer3();
	SYS_SysInit();
//  	init_struct();
	for(;;)
	{
 		SYS_RunTask();
	}
}
// eof peer2Peer.c
