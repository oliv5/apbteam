/* adc.host.c */
/* avr.adc - AVR ADC module. {{{
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
#include "adc.h"

#include "modules/utils/utils.h"

/** Simulate hardware register. */
uint16_t ADC;

/** Table that the user can update to simulate an ADC. */
uint16_t adc_values[8];

/** Initialise ADC. */
void
adc_init (void)
{
    /* Nothing. */
}

/** Choose and start measure on ADC line. */
void
adc_start (uint8_t c)
{
    assert (c < UTILS_COUNT (adc_values));
    ADC = adc_values[c];
}

