/* network_specific.h */
/* Beacon specifical network management. {{{
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

#ifndef _NETWORK_SPECIFIC_H
#define _NETWORK_SPECIFIC_H

#include <zdo.h>
#include "network.h"

#define NETWORK_ACTIVITY_LED 2


/* Specific callback after data packet received */
void network_specific_DataIndicationcallback(APS_DataInd_t* indData);

/* Specific callback after data packet sent */
void network_specific_DataConfcallback(void);

#endif
