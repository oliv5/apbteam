/* led.c */
/* Led management. {{{
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

#include <appTimer.h>
#include "led.h"

// Timer indicating starting network during network joining.
// Also used as a delay timer between APS_DataConf and APS_DataReq.
static HAL_AppTimer_t delayTimer;


/* This function display a combination of led according the the given state */
void led_display_state(int state)
{
	switch(state)
	{
		case APP_INITIAL_STATE:
		case APP_NETWORK_JOINING_STATE:
			led_on(1);
			led_off(2);
			break;
		case APP_NETWORK_JOINED_STATE:
			led_on(1);
			led_on(2);
		default:
			break;
	}
}


/* This function initializes the leds*/
void init_led(void)
{
	/*D5/D6/D7 are configured in output */
	DDRD = 0xE0;
}

/* This function activates a specific led */
void led_on(int number)
{
	switch(number)
	{
		case 1:
			PORTD|=0x20;
			break;
		case 2:
			PORTD|=0x40;
			break;
		case 3:
			PORTD|=0x80;
			break;
		default:
			break;
	}
}

/* This function deactivates a specific led*/
void led_off(int number)
{
	switch(number)
	{
		case 1:
			PORTD&=0xDF;
			break;
		case 2:
			PORTD&=0xBF;
			break;
		case 3:
			PORTD&=0x7F;
			break;
		default:
			break;
	}
}

/* This function inverse the state of a specific led*/
void led_inverse(int number)
{
	switch(number)
	{
		case 1:
 			PORTD^=0x20;
			break;
		case 2:
 			PORTD^=0x40;
			break;
		case 3:
 			PORTD^=0x80;
			break;
		default:
			break;
	}
}

/* This function enables the timer used by the network blink status led */
void led_start_blink(void)
{
	delayTimer.interval = APP_JOINING_INDICATION_PERIOD;
	delayTimer.mode     = TIMER_REPEAT_MODE;
	delayTimer.callback = led_network_status_blink_callback;
	HAL_StartAppTimer(&delayTimer);
}

/* This function disables the timer used by the network blink status led */
void led_stop_blink(void)
{
	HAL_StopAppTimer(&delayTimer);
	led_on(1);
}

/*  Led blink callback*/
void led_network_status_blink_callback(void)
{
	led_inverse(1);
}
