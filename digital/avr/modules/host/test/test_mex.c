/* test_mex.c */
/* avr.host - Host fonctions modules. {{{
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
#include "common.h"
#include "modules/host/host.h"
#include "modules/host/mex.h"

#include <stdio.h>

void
a82 (void *user, mex_msg_t *msg)
{
    printf ("oucouc\n");
    u8 nb;
    mex_msg_pop (msg, "B", &nb);
    nb++;
    mex_msg_t *m = mex_msg_new (mex_msg_mtype (msg));
    mex_msg_push (m, "B", nb);
    mex_node_response (m);
}

void
a801 (void *user, mex_msg_t *msg)
{
    printf ("coucou\n");
    u8 b;
    u16 h;
    u32 l;
    mex_msg_pop (msg, "BHL", &b, &h, &l);
    if (mex_msg_mtype (msg) == 0x80)
	assert (b == 1 && h == 2 && l == 3);
    else
	assert (b == 4 && h == 5 && l == 6);
}

int
main (int argc, char **argv)
{
    int i;
    printf ("reset\n");
    host_init (argc, argv);
    if (argc != 2 || !((argv[1][0] == '1' || argv[1][0] == '2')
		       && argv[1][1] == '\0'))
      {
	fprintf (stderr, "%s 1|2\n", argv[0]);
	return 1;
      }
    if (argv[1][0] == '1')
      {
	mex_node_register (0x82, a82, NULL);
	mex_node_connect ();
	i = host_fetch_integer ("reseted");
	if (i == -1)
	  {
	    mex_msg_t *m = mex_msg_new (0x80);
	    mex_msg_push (m, "BHL", 1, 2, 3);
	    mex_node_send (m);
	    host_register_integer ("reseted", 1);
	    host_reset ();
	  }
	else
	  {
	    mex_msg_t *m = mex_msg_new (0x81);
	    mex_msg_push (m, "BHL", 4, 5, 6);
	    mex_node_send (m);
	    mex_node_wait ();
	  }
      }
    else
      {
	mex_node_register (0x80, a801, NULL);
	mex_node_register (0x81, a801, NULL);
	mex_node_connect ();
	mex_msg_t *m = mex_msg_new (0x82);
	mex_msg_push (m, "B", 42);
	mex_msg_t *r = mex_node_request (m);
	assert (mex_msg_mtype (r) == 0x82);
	u8 rb;
	mex_msg_pop (r, "B", &rb);
	assert (rb == 43);
	mex_node_wait_date (42);
	mex_node_wait ();
      }
    return 0;
}
