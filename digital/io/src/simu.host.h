#ifndef simu_host_h
#define simu_host_h
/* simu.host.h - Host simulation. */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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

#ifdef HOST

/** Hooked, initialise the host simulation. */
void
main_timer_init (void);

/** Hooked, as before, wait for the next iteration. */
void
main_timer_wait (void);

/** Hooked, do nothing. */
void
switch_init (void);

/** Hooked, request via mex. */
uint8_t
switch_get_color (void);

/** Hooked, request via mex. */
uint8_t
switch_get_jack (void);

/** Hooked, do nothing. */
void
chrono_init (void);

#endif /* defined (HOST) */

#endif /* simu_host_h */
