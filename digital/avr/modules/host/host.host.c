/* host.host.c */
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
#define _GNU_SOURCE
#include "common.h"
#include "host.h"

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Saved arguments.  Uninitialised global variables are set to 0 by the
 * compiler. */
static int host_saved_argc;
static char **host_saved_argv;

/* User arguments not used by this module. */
static int host_user_argc;
static char **host_user_argv;

/* Instance string, from program arguments. */
static char *host_instance;

/** Initialise host module. */
void
host_init (int argc, char **argv)
{
    host_saved_argc = argc;
    host_saved_argv = argv;
    /* Get instance argument. */
    argv++;
    argc--;
    if (argc && strncmp (argv[0], "-i", 2) == 0)
      {
	host_instance = argv[0] + 2;
	argv++;
	argc--;
      }
    /* Save other arguments. */
    host_user_argc = argc;
    host_user_argv = argv;
}

/** Retrieve saved program arguments.  Program name and used parameters are
 * stripped. */
void
host_get_program_arguments (int *argc, char ***argv)
{
    assert (host_saved_argc);
    *argc = host_user_argc;
    *argv = host_user_argv;
}

/** Retrieve instance given on command line, or use default.  Strip tail
 * components if requested.  Buffer is valid until next call. */
const char *
host_get_instance (const char *def, int strip)
{
    if (!host_instance)
	return def;
    else if (strip == 0)
	return host_instance;
    else
      {
	static char stripped[256];
	assert (strlen (host_instance) < sizeof (stripped));
	strcpy (stripped, host_instance);
	while (strip--)
	  {
	    char *p = strrchr (stripped, ':');
	    if (!p)
		return "";
	    else
		*p = '\0';
	  }
	return stripped;
      }
}

/** Register a host integer. */
void
host_register_integer (const char *name, int val)
{
    int r;
    char sval[256];
    /* Convert to string, and register the string. */
    r = snprintf (sval, sizeof (sval), "%d", val);
    assert (r < (int) sizeof (sval));
    host_register_string (name, sval);
}

/** Register a host string. */
void
host_register_string (const char *name, const char *val)
{
    int r;
    r = setenv (name, val, 1);
    assert (r == 0);
}

/** Fetch a host integer, return -1 if non existant. */
int
host_fetch_integer (const char *name)
{
    const char *sval = host_fetch_string (name);
    return sval ? atoi (sval) : -1;
}

/** Fetch a host string, return 0 if non existant. */
const char *
host_fetch_string (const char *name)
{
    return getenv (name);
}

/** Reset the program. */
void
host_reset (void)
{
    assert (host_saved_argv);
    execv (program_invocation_name, host_saved_argv);
    assert_perror (errno);
    abort ();
}

