/* test_host.c */
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
#include "common.h"
#include "modules/host/host.h"

#include <string.h>
#include <stdio.h>

int
main (int argc, char **argv)
{
    int i;
    const char *s;
    int ac;
    char **av;
    printf ("reset\n");
    host_init (argc, argv);
    i = host_fetch_integer ("avr_integer");
    s = host_fetch_string ("avr_string");
    if (i != -1)
      {
	printf ("get\n");
	host_get_program_arguments (&ac, &av);
	assert_print (ac == 1 && strcmp (av[0], "ni") == 0,
		      "argument passing not working");
	assert (i == 42);
	assert (strcmp (s, "Ni!") == 0);
      }
    else
      {
	printf ("set\n");
	assert_print (argc == 2 && strcmp (argv[1], "ni") == 0,
		      "please provide \"ni\" as the first argument");
	host_register_integer ("avr_integer", 42);
	host_register_string ("avr_string", "Ni!");
	host_reset ();
      }
    return 0;
}
