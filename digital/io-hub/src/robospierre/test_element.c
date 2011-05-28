/* test_element.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2011 Jérôme Jutteau
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
#include "element.h"

#include "chrono.h"

#include <stdio.h>

#define TEST_PRINT_TYPE_SCORE_PICK 0
#define TEST_PRINT_TYPE_SCORE_UNLOAD 1
#define TEST_PRINT_TYPE_ELEMENT 2
int test_print_type_ = TEST_PRINT_TYPE_SCORE_PICK;
position_t test_robot_pos_ = {{0, 2100}, 1};

enum team_color_e team_color;

/** Stubbed chrono. */
uint32_t test_chrono_ms;

void
chrono_init (void)
{
    test_chrono_ms = CHRONO_MATCH_DURATION_MS;
}

uint32_t
chrono_remaining_time (void)
{
    return test_chrono_ms;
}

uint8_t
test_element_convert (const char *l, uint8_t number)
{
    switch (l[0])
      {
      case 'A':
	switch (number)
	  {
	  case 1: return (31);
	  case 3: return (32);
	  case 5: return (33);
	  case 7: return (34);
	  case 9: return (35);
	  case 11: return (36);
	  }
      case 'B':
	switch (number)
	  {
	  case 2: return (0);
	  case 4: return (2);
	  case 8: return (3);
	  case 10: return (1);
	  }
      case 'C':
	switch (number)
	  {
	  case 1: return (37);
	  case 3: return (38);
	  case 5: return (39);
	  case 7: return (40);
	  case 9: return (41);
	  case 11: return (42);
	  }
      case 'D':
	switch (number)
	  {
	  case 0: return (21);
	  case 2: return (4);
	  case 4: return (6);
	  case 8: return (7);
	  case 10: return (5);
	  case 12: return (22);
	  }
      case 'E':
	switch (number)
	  {
	  case 1: return (43);
	  case 3: return (44);
	  case 5: return (45);
	  case 7: return (46);
	  case 9: return (47);
	  case 11: return (48);
	  }
      case 'F':
	switch (number)
	  {
	  case 0: return (23);
	  case 12: return (24);
	  }
      case 'G':
	switch (number)
	  {
	  case 2: return (8);
	  case 4: return (10);
	  case 6: return (20);
	  case 8: return (11);
	  case 10: return (9);
	  }
      case 'H':
	switch (number)
	  {
	  case 0: return (25);
	  case 1: return (49);
	  case 3: return (50);
	  case 5: return (51);
	  case 7: return (52);
	  case 9: return (53);
	  case 11: return (54);
	  case 12: return (26);
	  }
      case 'I':
	switch (number) /*12*/
	  {
	  case 2: return (12);
	  case 4: return (14);
	  case 8: return (15);
	  case 10: return (13);
	  }
      case 'J':
	switch (number)
	  {
	  case 0: return (27);
	  case 12: return (28);
	  }
      case 'K':
	switch (number)
	  {
	  case 1: return (55);
	  case 3: return (56);
	  case 5: return (57);
	  case 7: return (58);
	  case 9: return (59);
	  case 11: return (60);
	  }
      case 'L':
	switch (number)
	  {
	  case 2: return (16);
	  case 4: return (18);
	  case 8: return (19);
	  case 10: return (17);
	  }
      case 'M':
	switch (number)
	  {
	  case 0: return (29);
	  case 1: return (63);
	  case 3: return (64);
	  case 5: return (61);
	  case 7: return (62);
	  case 9: return (65);
	  case 11: return (66);
	  case 12: return (30);
	  }
      }
    return 0;
}

long int
test_element_get_score (uint8_t element_id)
{
    if (test_print_type_ == TEST_PRINT_TYPE_SCORE_PICK)
	return (long int) element_score (test_robot_pos_, element_id);
    else if (test_print_type_ == TEST_PRINT_TYPE_SCORE_UNLOAD)
	return (long int) element_unload_score (test_robot_pos_, element_id);
    else
	return (long int) element_table[element_id].type;
}

#define p(l, n) (test_element_print (#l, n))
long int
test_element_print (const char *l, uint8_t number)
{
    return test_element_get_score (test_element_convert (l, number));
}

void
test_element_print_table ()
{
    printf ("parameters:\n");
    printf ("- robot_pos = (%u, %u)\n", test_robot_pos_.v.x, test_robot_pos_.v.y);
    printf ("- side = %u\n", team_color);
    printf ("  _____0_________1_____2____3____4____5____6____7____8____9___10___11_________12_____ \n");
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("A|   Red     |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |   Blue    |\n", p (A, 1), p (A, 3), p (A, 5), p(A, 7), p (A, 9), p (A, 11));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("B|           |--------%5ld-----%5ld------+--------%5ld-----%5ld------|           |\n", p (B, 2), p (B, 4), p (B, 8), p (B, 10));
    printf (" |-----------+         |         |         |         |         |         +-----------|\n");
    printf (" |           |         |   [B]   |         |         |   [R]   |         |           |\n");
    printf ("C|           |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |           |\n", p (C, 1), p (C, 3), p (C, 5), p (C, 7), p(C, 9), p (C, 11));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("D|   %5ld   |--------%5ld-----%5ld------+--------%5ld-----%5ld------|   %5ld   |\n", p (D, 0), p (D, 2), p (D, 4), p (D, 8), p (D, 10), p (D, 12));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("E|           |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |           |\n", p (E, 1), p (E, 3), p (E, 5), p (E, 7), p(E, 9), p(E, 11));
    printf ("F|   %5ld   |         |         |         |         |         |         |   %5ld   |\n", p (F, 0), p(F, 12));
    printf ("G|           |--------%5ld-----%5ld-----%5ld-----%5ld-----%5ld------|           |\n", p (G, 2), p (G, 4), p(G, 6), p (G, 8), p (G, 10));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf (" |           |         |   [B]   |         |         |   [R]   |         |           |\n");
    printf ("H|   %5ld   |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |   %5ld   |\n", p(H, 0), p (H, 1), p (H, 3), p (H, 5), p (H, 7), p(H, 9), p(H, 11), p(H, 12));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("I|           |--------%5ld-----%5ld------+--------%5ld-----%5ld------|           |\n", p (I, 2), p (I, 4), p (I, 8), p (I, 10));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("J|   %5ld   |         |         |         |         |         |         |   %5ld   |\n", p (J, 0), p(J, 12));
    printf ("K|           |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |           |\n", p (K, 1), p (K, 3), p (K, 5), p (K, 7), p(K, 9), p(K, 11));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("L|           |--------%5ld-----%5ld------+--------%5ld-----%5ld------|           |\n", p (L, 2), p (L, 4), p (L, 8), p (L, 10));
    printf (" |           |         |         |         |         |         |         |           |\n");
    printf ("M|   %5ld   |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |  %5ld  |   %5ld   |\n", p(M, 0), p (M, 1), p (M, 3), p (M, 5), p (M, 7), p(M, 9), p(M, 11), p(M, 12));
    printf (" |           |###################|   [R]   |   [B]   |###################|           |\n");
    printf (" |           |###################|         |         |###################|           |\n");
    printf (" +-----------|###################+---------+---------+###################+-----------+\n");
}

int main ()
{
    int exit = 0;
    char cmd;
    int x;
    char y[10], z[10];
    int type = 0;
    chrono_init ();

    while (!exit)
      {
	element_init ();
	printf ("\ncommands:\n");
	printf ("s: print scores, ");
	printf ("p: print unload scores, ");
	printf ("e: print elements, ");
	printf ("t: set match time, ");
	printf ("c: set robot side, ");
	printf ("r: set robot position, ");
	printf ("i: pick up an element, ");
	printf ("n: indicate there is no element, ");
	printf ("m: id of nearest element, ");
	printf ("g: get position and angle of desired element,");
	printf ("q: quit\n");
	printf ("your choice: ");
	fflush (stdin);
	scanf ("%c", &cmd);
	switch (cmd)
	  {
	  case 's':
	    test_print_type_ = TEST_PRINT_TYPE_SCORE_PICK;
	    test_element_print_table ();
	    break;
	  case 'p':
	    test_print_type_ = TEST_PRINT_TYPE_SCORE_UNLOAD;
	    test_element_print_table ();
	    break;
	  case 'e':
	    test_print_type_ = TEST_PRINT_TYPE_ELEMENT;
	    test_element_print_table ();
	    break;
	  case 't':
	    printf ("set match time: ");
	    scanf ("%i", &test_chrono_ms);
	    break;
	  case 'c':
	    team_color ^= TEAM_COLOR_RIGHT;
	    printf ("Side is now %u\n", team_color);
	    break;
	  case 'r':
	    printf ("set robot position:\n");
	    printf ("number: ");
	    scanf ("%i", &x);
	    printf ("letter: ");
	    scanf ("%s", y);
	    test_robot_pos_.v = element_table[test_element_convert (y, x)].pos;
	    break;
	  case 'i':
	    printf ("pick up an element:\n");
	    printf ("number: ");
	    scanf ("%i", &x);
	    printf ("letter: ");
	    scanf ("%s", y);
	    printf ("type: (P=Pawn Q=Queen K=King) ");
	    fflush (stdin);
	    scanf ("%s", z);
	    if (z[0] == 'P')
		type = ELEMENT_PAWN;
	    else if (z[0] == 'Q')
		type = ELEMENT_QUEEN;
	    else if (z[0] == 'K')
		type = ELEMENT_KING;
	    else
		printf ("Unknown !\n");
	    element_taken (test_element_convert (y, x), type);
	    break;
	  case 'n':
	    printf ("indicate there no element\n");
	    printf ("number: ");
	    scanf ("%i", &x);
	    printf ("letter: ");
	    scanf ("%s", y);
	    element_not_here (test_element_convert (y, x));
	    break;
	  case 'm':
	    printf ("nearest element id from robot: %u\n",
		    element_nearest_element_id (test_robot_pos_));
	    break;
	  case 'g':
		printf ("element id: ");
		scanf ("%i", &x);
		position_t pos = element_get_pos (x);
		printf ("x: %u y: %u a: %u\n", pos.v.x, pos.v.y, pos.a);
	    break;
	  case 'q':
	    exit = 1;
	    break;
	  }
	fflush (stdin);
	scanf ("%c", &cmd);
      }
    return 0;
}

