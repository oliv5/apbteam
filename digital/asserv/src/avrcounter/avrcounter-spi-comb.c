/* avrcounter-spi-comb.c - Combined table, SPI output. */
/* avrcounter - Incremental encoder counter. {{{
 *
 * Copyright (C) 2007 Nicolas Schodet
 *
 * Robot APB Team 2008.
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
#include <io.h>

register volatile uint8_t count0 __asm__ ("r2"), count1 __asm__ ("r3");
register volatile uint8_t int_tmp0 __asm__ ("r4"), int_tmp1 __asm__ ("r5");

void
SIG_SPI (void) __attribute__ ((naked));
void
SIG_SPI (void)
{
    int_tmp0 = SPDR;
    int_tmp1 = count0;
    if (SPDR & 1)
	int_tmp1 = count1;
    SPDR = int_tmp1;
    __asm__ __volatile__ ("reti"::);
}

int
main (void)
{
    uint8_t state = 0;
    count0 = 0;
    count1 = 0;
    /* Encoder signals:
     * ____----____----____----
     * __----____----____----__  */
#define trans_0   0 /* 00 -> 00 */
#define trans_1  +1 /* 00 -> 01 */
#define trans_2  -1 /* 00 -> 10 */
#define trans_3   0 /* 00 -> 11, missed! */
#define trans_4  -1 /* 01 -> 00 */
#define trans_5   0 /* 01 -> 01 */
#define trans_6   0 /* 01 -> 10, missed! */
#define trans_7  +1 /* 01 -> 11 */
#define trans_8  +1 /* 10 -> 00 */
#define trans_9   0 /* 10 -> 01, missed! */
#define trans_10  0 /* 10 -> 10 */
#define trans_11 -1 /* 10 -> 11 */
#define trans_12  0 /* 11 -> 00, missed! */
#define trans_13 -1 /* 11 -> 01 */
#define trans_14 +1 /* 11 -> 10 */
#define trans_15  0 /* 11 -> 11 */
#define trans_x_y(x, y) { trans_ ## x, trans_ ## y },
#define trans_x_all(x) \
    trans_x_y (x, 0) \
    trans_x_y (x, 1) \
    trans_x_y (x, 2) \
    trans_x_y (x, 3) \
    trans_x_y (x, 4) \
    trans_x_y (x, 5) \
    trans_x_y (x, 6) \
    trans_x_y (x, 7) \
    trans_x_y (x, 8) \
    trans_x_y (x, 9) \
    trans_x_y (x, 10) \
    trans_x_y (x, 11) \
    trans_x_y (x, 12) \
    trans_x_y (x, 13) \
    trans_x_y (x, 14) \
    trans_x_y (x, 15)
    static const struct
      {
	int8_t inc0;
	int8_t inc1;
      } inc[] = {
	trans_x_all (0)
	trans_x_all (1)
	trans_x_all (2)
	trans_x_all (3)
	trans_x_all (4)
	trans_x_all (5)
	trans_x_all (6)
	trans_x_all (7)
	trans_x_all (8)
	trans_x_all (9)
	trans_x_all (10)
	trans_x_all (11)
	trans_x_all (12)
	trans_x_all (13)
	trans_x_all (14)
	trans_x_all (15)
    };
    DDRD = 0xff;
    while (1)
      {
	state = (state << 4) | (PINC & 0x0f);
	count0 += inc[state].inc0;
	count1 += inc[state].inc1;
      }
}
