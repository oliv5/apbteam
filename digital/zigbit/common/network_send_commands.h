/* network_send_commands.h */
/* generic messages to send. {{{
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

#ifndef _NETWORK_SEND_COMMANDS_H
#define _NETWORK_SEND_COMMANDS_H

/* This function must be used to send angle through zigbee network */
void network_send_angle(uint16_t address,uint16_t angle);

/* This function must be used to send uart frame over zigbee network */
void network_send_buffer_over_zb(uint16_t address,uint8_t * buffer,uint8_t len);

/* This function must be used to send reset command through zigbee network */
void network_send_reset(uint16_t address);

/* This function must be used to send jack state through zigbee network */
void network_send_jack_state(uint16_t address,uint8_t state);

#endif
