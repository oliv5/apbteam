/* trust.h */
/* Beacon Trust control {{{
 *
 * Copyright (C) 2011 Florent Duchon
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

#ifndef _TRUST_H
#define _TRUST_H

#define TRUST_MIN			0
#define TRUST_THRESHOLD		30
#define TRUST_MAX			100

/* Status returns */
typedef enum
{
	TRUST_LEVEL_OK,
	TRUST_TOO_BAD
} TTrustStatus;

/* This function returns the trust level */
TTrustStatus trust_check_level(void);

/* This function increases the trust level for a specifical obstacle */
TTrustStatus trust_increase(int number);

/* This function decreases the trust level for all obstacles */
TTrustStatus trust_decrease(void);


#endif