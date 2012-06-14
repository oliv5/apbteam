#ifndef adc_h
#define adc_h
/* adc.h */
/* avr.adc - AVR ADC module. {{{
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

/* Different name for AVR with auto-trigger support. */
#if !defined (__AVR_ATmega128__)
# define ADCSR ADCSRA
#endif

#ifdef HOST

/* No hardware on host. */
# define ADCSRA 1
# define ADIF 0

extern uint16_t ADC;

/** Table that the user can update to simulate an ADC. */
extern uint16_t adc_values[8];

#endif /* defined (HOST) */

/** Initialise ADC. */
void
adc_init (void);

/** Choose and start measure on ADC line. */
void
adc_start (uint8_t c);

/** Check if measure is finished. */
#define adc_checkf() bit_is_set (ADCSR, ADIF)

/** Read measure. */
#define adc_read() ADC

#endif /* adc_h */
