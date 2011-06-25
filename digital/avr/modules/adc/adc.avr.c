/* adc.avr.c */
/* n.avr.adc - AVR ADC Module. {{{
 *
 * Copyright (C) 2005 Thomas Burg
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
 * Contact :
 *      Email: burg AT efrei DOT fr
 * }}} */
#include "common.h"
#include "adc.h"

#include "modules/utils/utils.h"
#include "io.h"

/* Tested AVR check. */
#if defined (__AVR_ATmega8__)
#elif defined (__AVR_ATmega8535__)
#elif defined (__AVR_ATmega128__)
#elif defined (__AVR_ATmega64__)
#elif defined (__AVR_ATmega16__)
# elif defined (__AVR_AT90USB646__)
# elif defined (__AVR_AT90USB647__)
# elif defined (__AVR_AT90USB1286__)
# elif defined (__AVR_AT90USB1287__)
#else
# warning "adc: not tested on this chip."
#endif

#if defined (__AVR_ATmega128__)
#else
/* If the ADCSRB register is not set, auto-trigger is equivalent to
 * free-running mode. */
# define ADFR ADATE
#endif

/* ADC configuration. */
/* REFS = 01: AVCC with external capacitor at AREF pin.
 *        11: Internal 2.56V Voltage Reference with external capacitor
 *        at AREF pin. */
#define ADMUX_CFG (regv (REFS1, REFS0, ADLAR, MUX4, MUX3, MUX2, MUX1, MUX0, \
			     0,     1,     0,    0,    0,    0,    0,    0))
#define ADCSR_CFG (regv (ADEN, ADSC, ADFR, ADIF, ADIE, ADPS2, ADPS1, ADPS0, \
			    1,    0,    0,    1,    0,     0,     0,     0))
#define ADCSR_CFG_115200 7

/** Initialise ADC. */
void
adc_init (void)
{
    /* Low freq. */
    ADCSR = ADCSR_CFG | ADCSR_CFG_115200;
}

/** Choose and start measure on ADC line. */
void
adc_start (uint8_t c)
{
    /* Choose ADC. */
    ADMUX = ADMUX_CFG | (c & 0x07);
    /* Start measure. */
    ADCSR |= _BV (ADSC);
}

