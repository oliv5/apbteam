#ifndef adc_h
#define adc_h
/* adc.h */
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
/* Different name on ATmega8535. */

#if defined (__AVR_ATmega8535__)
#define ADCSR ADCSRA
#endif

/* +AutoDec */

/** Initialise adc. */
void
adc_init (void);

/** Choose and start mesure on adc line. */
void
adc_start (uint8_t c);

/* -AutoDec */

/** check on finish mesure */
#define adc_checkf() bit_is_set (ADCSR, ADIF)

/** read mesure */
#define adc_read() ADCW

#endif /* adc_h */
