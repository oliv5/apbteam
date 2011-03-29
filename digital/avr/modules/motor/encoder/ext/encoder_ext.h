#ifndef encoder_ext_h
#define encoder_ext_h
/* encoder_ext.h */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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

/** See encoder_init. */
void
encoder_ext_init (uint8_t index, encoder_t *encoder);

/** See encoder_update_step. */
void
encoder_ext_update_step (void);

/** See encoder_update. */
void
encoder_ext_update (void);

#if AC_ENCODER_EXT_EXPORT_READ
uint8_t
encoder_ext_read (uint8_t n);
#endif

#endif /* encoder_ext_h */
