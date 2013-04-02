/* buttons.c */
/*lol-v2 buttons configuraiton. {{{
 *
 * Copyright (C) 2013 Florent Duchon
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

#include "buttons.h"

/* This function initializes buttons */
void buttons_init(void)
{
	/* PORTD 0->3 as inputs */
	DDRD |= 0x0F;
	
	/* Activate internal pull up */
	PORTD |= 0x0F;
}

/* This function retuns the button state according to its number */
TButton_state button_get_state(uint8_t number)
{
	TButton_state state;
	
	switch(number)
	{
		case 1:
			if(!(PIND&0x08))
				state = BUTTON_PRESSED;
			else
				state = BUTTON_RELEASED;
			break;
		case 2:
			if(!(PIND&0x04))
				state = BUTTON_PRESSED;
			else
				state = BUTTON_RELEASED;
			break;
		case 3: 
			if(!(PIND&0x02))
				state = BUTTON_PRESSED;
			else
				state = BUTTON_RELEASED;
			break;
		case 4:
			if(!(PIND&0x01))
				state = BUTTON_PRESSED;
			else
				state = BUTTON_RELEASED;
			break;
		default:
			/* do nothing */
			break;
	}
	return state;
}