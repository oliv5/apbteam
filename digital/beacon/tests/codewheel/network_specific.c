/* network.c */
/* Beacon network management. {{{
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

#include <types.h>
#include <configServer.h>
#include <zdo.h>

#include "configuration.h"
#include "network.h"
#include "network_specific.h"
#include "print.h"
AppMessageBuffer_t zigbit_tx_buffer;

extern APS_RegisterEndpointReq_t endpointParams;
extern APS_DataReq_t network_config;





/* Specific callback after data packet sent */
void network_specific_DataConfcallback(void)
{

}
