#ifndef bottom_clamp_h
#define bottom_clam_h
/* bottom_clam.h */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Julien Balleyguier
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

/*function to put the clamp in the "clam_mode" (i.e putting the global var ctx.calm_mode at 1)*/
void clamp_calm_mode(int mode);

/*function to find out how many times the clamp has been blocked*/
uint8_t clamp_read_blocked_cpt(void);

/** Request a specific move to be executed now or as soon as going to IDLE
 * state. */
void
clamp_request (uint16_t event);

#endif /* bottom_clamp_h */
