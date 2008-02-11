#ifndef utils_h
#define utils_h
/* utils.h */
/*  {{{
 *
 * Copyright (C) 2008 Pierre Prot
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


/* Concatenate 8 bits values.
 *  vx: bit value
 *  bx: bit shift
 */
#define regv(b0, b1, b2, b3, b4, b5, b6, b7, v0, v1, v2, v3, v4, v5, v6, v7) \
    ( \
        ((v0 && 1) << b0) | \
        ((v1 && 1) << b1) | \
        ((v2 && 1) << b2) | \
        ((v3 && 1) << b3) | \
        ((v4 && 1) << b4) | \
        ((v5 && 1) << b5) | \
        ((v6 && 1) << b6) | \
        ((v7 && 1) << b7)   \
    )

#endif
