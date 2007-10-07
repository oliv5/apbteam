#ifndef host_h
#define host_h
/* host.h */
/* avr.host - Host fonctions modules. {{{
 *
 * Copyright (C) 2005-2006 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
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

/* Discourage use of this header in non HOST compilation. */
#ifndef HOST
# error Use this header only for HOST only files !
#endif

/** Initialise host module. */
void
host_init (int argc, char **argv);

/** Retrieve saved program arguments.  Program name and used parameters are
 * stripped. */
void
host_get_program_arguments (int *argc, char ***argv);

/** Host variables are usefull on reset.  They are passed in the environment.
 * This is not optimised for performance. */

/** Register a host integer. */
void
host_register_integer (const char *name, int val);

/** Register a host string. */
void
host_register_string (const char *name, const char *val);

/** Fetch a host integer, return -1 if non existant. */
int
host_fetch_integer (const char *name);

/** Fetch a host string, return 0 if non existant. */
const char *
host_fetch_string (const char *name);

/** Reset the program. */
void
host_reset (void) __attribute__ ((noreturn));

#endif /* host_h */
