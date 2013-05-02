//  {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "lcd.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/utils/delay.hh"
#include <libopencm3/stm32/f4/rcc.h>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN
                                 | RCC_AHB1ENR_IOPBEN | RCC_AHB1ENR_IOPCEN);
    LCD lcd;
    ucoo::delay (1);
    lcd.clear (LCD::color (255, 0, 0));
    ucoo::delay (1);
    lcd.clear (LCD::color (0, 255, 0));
    ucoo::delay (1);
    lcd.clear (LCD::color (0, 0, 255));
    ucoo::delay (1);
    lcd.rainbow ();
    while (1)
        ;
}
