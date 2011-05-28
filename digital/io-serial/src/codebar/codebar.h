#ifndef codebar_h
#define codebar_h
/* codebar.h */
/*  {{{
 *
 * Copyright (C) 2011
 *
 * Robot APB Team/Efrei 2011
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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

/* Read a string from UART0 and return NULL or the string read. */
char* read_string(int uart_port); 

/* Take a string and check it againt the list of valid element
 * as defined in io-hub/src/robospierre/element.h and return the
 * u8 value of this element. 
 */
uint8_t string_to_element(char* data);

void proto_callback (uint8_t cmd, uint8_t size, uint8_t *args);

#endif /* codebar_h */
