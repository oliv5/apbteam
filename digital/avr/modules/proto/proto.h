#ifndef proto_h
#define proto_h
/* proto.h */
/*  {{{
 *
 * Copyright (C) 2004 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2005.
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

/** Protocol callback function.  Take the command and the arguments.  Must be
 * defined by the user. */
void
AC_PROTO_CALLBACK (uint8_t cmd, uint8_t size, uint8_t *args);

/** Protocol putc function.  Take a char to send.  Must be defined by the
 * user. */
void
AC_PROTO_PUTC (uint8_t c);

/* +AutoDec */

/** Accept a new character. */
void
proto_accept (uint8_t c);

/* Send a argument byte. */
void
proto_arg (uint8_t a);

/** Send a command, generic function. */
void
proto_send (uint8_t cmd, uint8_t size, uint8_t *args);

/* -AutoDec */

#include "proto_inline.c"

#endif /* proto_h */
