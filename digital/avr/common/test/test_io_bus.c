/* test_io_bus.c */
/* avr.modules - AVR modules. {{{
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
#include "common.h"
#include "io_bus.h"

#include "modules/math/random/random.h"

#include <stdio.h>

/* Use non existents ports. */
#define TEST_PORTS 12
volatile uint8_t PORTU[TEST_PORTS], DDRU[TEST_PORTS], PINU[TEST_PORTS];

#define TEST_BUSES 7
#define TEST_SLOTS 11
#define TEST_BUS_0 U[0], 4, 0
#define TEST_BUS_1 U[1], 4, 2
#define TEST_BUS_2 U[2], 4, 4
#define TEST_BUS_3 U[3], 4, 0, U[4], 4, 0
#define TEST_BUS_4 U[5], 4, 0, U[6], 2, 0, U[7], 2, 6
#define TEST_BUS_5 U[8], 3, 1, U[9], 2, 0, U[10], 3, 0
#define TEST_BUS_6 U[11], 8, 0

#define TEST_OUTER_LOOPS 10
#define TEST_INNER_LOOPS 10

/* Table of unused bit masks. */
static const uint8_t unused_mask[TEST_PORTS] = { 0xf0, 0xc3, 0x0f, 0xf0, 0xf0,
    0xf0, 0xfc, 0x3f, 0xf1, 0xfc, 0xf8, 0x00 };

/* Make a test iteration, set input or output, rotate bus values. */
void
test_io_bus_iteration (uint8_t out)
{
    if (out)
      {
	IO_BUS_OUTPUT (TEST_BUS_0);
	IO_BUS_OUTPUT (TEST_BUS_1);
	IO_BUS_OUTPUT (TEST_BUS_2);
	IO_BUS_OUTPUT (TEST_BUS_3);
	IO_BUS_OUTPUT (TEST_BUS_4);
	IO_BUS_OUTPUT (TEST_BUS_5);
	IO_BUS_OUTPUT (TEST_BUS_6);
      }
    else
      {
	IO_BUS_INPUT (TEST_BUS_0);
	IO_BUS_INPUT (TEST_BUS_1);
	IO_BUS_INPUT (TEST_BUS_2);
	IO_BUS_INPUT (TEST_BUS_3);
	IO_BUS_INPUT (TEST_BUS_4);
	IO_BUS_INPUT (TEST_BUS_5);
	IO_BUS_INPUT (TEST_BUS_6);
      }
    IO_BUS_SET (TEST_BUS_0, IO_BUS_GET (TEST_BUS_1));
    IO_BUS_SET (TEST_BUS_1, IO_BUS_GET (TEST_BUS_2));
    IO_BUS_SET (TEST_BUS_2, IO_BUS_GET (TEST_BUS_3) & 0xf);
    IO_BUS_SET (TEST_BUS_3, IO_BUS_GET (TEST_BUS_3) >> 4 | (IO_BUS_GET (TEST_BUS_4) & 0xf) << 4);
    IO_BUS_SET (TEST_BUS_4, IO_BUS_GET (TEST_BUS_4) >> 4 | (IO_BUS_GET (TEST_BUS_5) & 0xf) << 4);
    IO_BUS_SET (TEST_BUS_5, IO_BUS_GET (TEST_BUS_5) >> 4 | (IO_BUS_GET (TEST_BUS_6) & 0xf) << 4);
    IO_BUS_SET (TEST_BUS_6, IO_BUS_GET (TEST_BUS_6) >> 4 | IO_BUS_GET (TEST_BUS_0) << 4);
}

unsigned long
test_io_bus (void)
{
    uint8_t i, j, k;
    unsigned long failed = 0;
    for (k = 0; k < TEST_PORTS; k++)
	PORTU[k] = DDRU[k] = PINU[k] = 0;
    for (i = 0; i < TEST_OUTER_LOOPS; i++)
      {
	uint8_t buses[TEST_BUSES];
	/* Randomize buses. */
	for (k = 0; k < TEST_BUSES; k++)
	    buses[k] = random_u32 () & 0xff;
	IO_BUS_SET (TEST_BUS_0, buses[0]);
	buses[0] &= 0xf;
	IO_BUS_SET (TEST_BUS_1, buses[1]);
	buses[1] &= 0xf;
	IO_BUS_SET (TEST_BUS_2, buses[2]);
	buses[2] &= 0xf;
	IO_BUS_SET (TEST_BUS_3, buses[3]);
	IO_BUS_SET (TEST_BUS_4, buses[4]);
	IO_BUS_SET (TEST_BUS_5, buses[5]);
	IO_BUS_SET (TEST_BUS_6, buses[6]);
	for (j = 0; j < TEST_SLOTS * TEST_OUTER_LOOPS; j++)
	  {
	    /* Randomize unused ports. */
	    uint8_t ports[TEST_PORTS];
	    for (k = 0; k < TEST_PORTS; k++)
	      {
		ports[k] = random_u32 ();
		PORTU[k] = (PORTU[k] & ~unused_mask[k])
		    | (ports[k] & unused_mask[k]);
		PINU[k] = PORTU[k];
		DDRU[k] = ports[k];
	      }
	    /* Rotate. */
	    test_io_bus_iteration (j & 1);
	    for (k = 0; k < TEST_PORTS; k++)
		PINU[k] = PORTU[k];
	    /* Check direction. */
	    for (k = 0; k < TEST_PORTS; k++)
		if (DDRU[k] != (0xff & (((j & 1) ? ~unused_mask[k] : 0)
					| (ports[k] & unused_mask[k]))))
		    failed++;
	    /* Check unused ports. */
	    for (k = 0; k < TEST_PORTS; k++)
		if ((PORTU[k] & unused_mask[k])
		    != (ports[k] & unused_mask[k]))
		    failed++;
	  }
	/* Check buses. */
	uint8_t get;
	get = IO_BUS_GET (TEST_BUS_0);
	if (get != buses[0])
	    failed++;
	get = IO_BUS_GET (TEST_BUS_1);
	if (get != buses[1])
	    failed++;
	get = IO_BUS_GET (TEST_BUS_2);
	if (get != buses[2])
	    failed++;
	get = IO_BUS_GET (TEST_BUS_3);
	if (get != buses[3])
	    failed++;
	get = IO_BUS_GET (TEST_BUS_4);
	if (get != buses[4])
	    failed++;
	get = IO_BUS_GET (TEST_BUS_5);
	if (get != buses[5])
	    failed++;
	get = IO_BUS_GET (TEST_BUS_6);
	if (get != buses[6])
	    failed++;
      }
    return failed;
}

/* Use real ports for assembly comparison. */
#ifdef HOST
volatile uint8_t PORTA, PORTB, DDRA, DDRB, PINA, PINB;
#endif
#define TEST_BUS_A1 A, 8, 0
#define TEST_BUS_A2 A, 4, 0, B, 4, 0
#define TEST_BUS_A3 A, 4, 2, B, 2, 4

/* This is not supposed to be run, used to compare assembly with next
 * function. */
uint8_t
test_asm_bus (uint8_t n)
{
    IO_BUS_OUTPUT (TEST_BUS_A1);
    IO_BUS_OUTPUT (TEST_BUS_A2);
    IO_BUS_OUTPUT (TEST_BUS_A3);
    IO_BUS_INPUT (TEST_BUS_A1);
    IO_BUS_INPUT (TEST_BUS_A2);
    IO_BUS_INPUT (TEST_BUS_A3);
    IO_BUS_SET (TEST_BUS_A1, n);
    IO_BUS_SET (TEST_BUS_A2, n);
    IO_BUS_SET (TEST_BUS_A3, n);
    IO_BUS_SET (TEST_BUS_A1, 42);
    IO_BUS_SET (TEST_BUS_A2, 42);
    IO_BUS_SET (TEST_BUS_A2, 2);
    IO_BUS_SET (TEST_BUS_A3, 42);
    uint8_t v = IO_BUS_GET (TEST_BUS_A1);
    v ^= IO_BUS_GET (TEST_BUS_A2);
    v ^= IO_BUS_GET (TEST_BUS_A3);
    return v;
}

/* This is not supposed to be run, used to compare assembly with previous
 * function. */
uint8_t
test_asm_manual (uint8_t n)
{
    DDRA = 0xff;
    DDRA |= 0x0f; DDRB |= 0x0f;
    DDRA |= 0x3c; DDRB |= 0x30;
    DDRA = 0x00;
    DDRA &= 0xf0; DDRB &= 0xf0;
    DDRA &= 0xc3; DDRB &= 0xcf;
    PORTA = n;
    PORTA = (PORTA & 0xf0) | (n & 0x0f);
    PORTB = (PORTB & 0xf0) | (n >> 4 & 0x0f);
    PORTA = (PORTA & 0xc3) | (n & 0x0f) << 2;
    PORTB = (PORTB & 0xcf) | (n & 0x30);
    PORTA = 42;
    PORTA = (PORTA & 0xf0) | (42 & 0x0f);
    PORTB = (PORTB & 0xf0) | (42 & 0xf0) >> 4;
    PORTA = (PORTA & 0xf0) | (2 & 0x0f);
    PORTB = (PORTB & 0xf0) | (2 & 0xf0) >> 4;
    PORTA = (PORTA & 0xc3) | (42 & 0x0f) << 2;
    PORTB = (PORTB & 0xcf) | (42 & 0x30);
    uint8_t v = PINA;
    v ^= (PINA & 0xf) | (PINB & 0xf) << 4;
    v ^= (PINA & 0x3c) >> 2 | (PINB & 0x30);
    return v;
}

int
main (void)
{
    unsigned long failed = test_io_bus ();
    printf ("failed %lu\n", failed);
    return failed ? 1 : 0;
}
