// io-hub - Modular Input/Output. {{{
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
#include "chrono.hh"

#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/timer.h>

Chrono::Chrono (int duration_ms)
    : duration_ms_ (duration_ms), started_ (false)
{
}

void
Chrono::start ()
{
    started_ = true;
    rcc_peripheral_enable_clock (&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);
    TIM2_PSC = 2 * rcc_ppre1_frequency / 1000 - 1; // 1 ms prescaler
    TIM2_ARR = 3600 * 1000;
    TIM2_EGR = TIM_EGR_UG;
    TIM2_CR1 = TIM_CR1_OPM | TIM_CR1_CEN;
}

int
Chrono::remaining_time_ms () const
{
    int now = 0;
    if (started_)
        now = TIM2_CNT;
    return duration_ms_ - now;
}
