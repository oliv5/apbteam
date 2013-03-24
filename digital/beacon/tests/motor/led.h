/* led.h */
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
#ifndef _LED_H
#define _LED_H

#define APP_JOINING_INDICATION_PERIOD     500L // Period of blinking during starting network

/* This function initializes the leds*/
void init_led(void);

/* This function activates a specific led */
void led_on(int number);

/* This function deactivates a specific led*/
void led_off(int number);

/* This function inverse the state of a specific led*/
void led_inverse(int number);

/* This function display a combination of led according the the given state */
void led_display_state(int state);

/* This function enables the timer used by the network blink status led */
void led_start_blink(void);

/* This function disables the timer used by the network blink status led */
void led_stop_blink(void);

/*  Led blink callback*/
void led_network_status_blink_callback(void);

#endif
