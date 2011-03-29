/* encoder_ext.c */
/* motor - Motor control module. {{{
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
#include "modules/motor/encoder/encoder.h"
#include "modules/utils/utils.h"
#include "encoder_ext.h"

#include "io_bus.h"

/**
 * This file add support for an external counter like the hdlcounter or
 * avrcounter project.  They are connected using the external memory
 * interface using the dedicated external memory hardware or classic IO.
 */

#if AC_ENCODER_EXT_USE_XMEM
# if (defined (AC_ENCODER_EXT_AD_BUS) \
      || defined (AC_ENCODER_EXT_ALE_IO) \
      || defined (AC_ENCODER_EXT_RD_IO) \
      || defined (AC_ENCODER_EXT_WR_IO))
#  error "motor/encoder/ext: when using XMEM, io definition is not used"
# endif
# if defined (__AVR_ATmega64__)
# elif defined (__AVR_ATmega128__)
# else
#  error "motor/encoder/ext: XMEM not tested on this chip"
# endif
#endif

/** External encoder state. */
struct encoder_ext_t
{
    /** Base encoder. */
    encoder_t *encoder;
    /** Encoder value at previous update. */
    uint16_t old;
    /** Encoder value accumulated during update steps. */
    uint16_t step_acc;
    /** Encoder value at previous update step. */
    uint8_t step_old;
};
typedef struct encoder_ext_t encoder_ext_t;

/** Global encoder states. */
encoder_ext_t encoder_ext[AC_ENCODER_EXT_NB];

/** Read an external encoder. */
#if !AC_ENCODER_EXT_EXPORT_READ
static inline
#endif
uint8_t
encoder_ext_read (uint8_t n)
{
#if AC_ENCODER_EXT_USE_XMEM
    uint8_t * const ext = (void *) (RAMEND + 1);
    return ext[n];
#else
    uint8_t v;
    IO_BUS_SET (AC_ENCODER_EXT_AD_BUS, n);
    IO_CLR (AC_ENCODER_EXT_ALE_IO);
    IO_BUS_SET (AC_ENCODER_EXT_AD_BUS, 0);
    IO_BUS_INPUT (AC_ENCODER_EXT_AD_BUS);
    IO_CLR (AC_ENCODER_EXT_RD_IO);
    utils_nop ();
    utils_nop ();
    v = IO_BUS_GET (AC_ENCODER_EXT_AD_BUS);
    IO_SET (AC_ENCODER_EXT_RD_IO);
    IO_SET (AC_ENCODER_EXT_ALE_IO);
    IO_BUS_OUTPUT (AC_ENCODER_EXT_AD_BUS);
    return v;
#endif
}

/** Initialize encoder bus, to be done once. */
static void
encoder_ext_init_bus (void)
{
    static uint8_t inited;
    if (!inited)
      {
	/* Setup XMEM or regular IO bus. */
#if AC_ENCODER_EXT_USE_XMEM
	/* Long wait-states. */
	XMCRA = _BV (SRW11);
	/* Do not use port C for address. */
	XMCRB = _BV (XMM2) | _BV (XMM1) | _BV (XMM0);
	/* Long wait-states and enable. */
	MCUCR |= _BV (SRE) | _BV (SRW10);
#else
	IO_SET (AC_ENCODER_EXT_ALE_IO);
	IO_SET (AC_ENCODER_EXT_RD_IO);
#ifdef AC_ENCODER_EXT_WR_IO
	IO_SET (AC_ENCODER_EXT_WR_IO);
#endif
	IO_OUTPUT (AC_ENCODER_EXT_ALE_IO);
	IO_OUTPUT (AC_ENCODER_EXT_RD_IO);
#ifdef AC_ENCODER_EXT_WR_IO
	IO_OUTPUT (AC_ENCODER_EXT_WR_IO);
#endif
	IO_BUS_SET (AC_ENCODER_EXT_AD_BUS, 0);
	IO_BUS_OUTPUT (AC_ENCODER_EXT_AD_BUS);
#endif
	/* Done. */
	inited = 1;
      }
}

void
encoder_ext_init (uint8_t index, encoder_t *encoder)
{
    /* Need a working bus. */
    encoder_ext_init_bus ();
    /* Keep encoder structure for future usage. */
    encoder_ext[index].encoder = encoder;
    /* Begin with safe values. */
    encoder_ext[index].step_old = encoder_ext_read (index);
}

void
encoder_ext_update_step (void)
{
    uint8_t i;
    uint8_t step_now[AC_ENCODER_EXT_NB];
    int8_t diff;
    /* Sample encoders. */
    for (i = 0; i < AC_ENCODER_EXT_NB; i++)
	step_now[i] = encoder_ext_read (i);
    /* Update step. */
    for (i = 0; i < AC_ENCODER_EXT_NB; i++)
      {
	diff = (int8_t) (step_now[i] - encoder_ext[i].step_old);
	encoder_ext[i].step_old = step_now[i];
	encoder_ext[i].step_acc += diff;
      }
}

void
encoder_ext_update (void)
{
    uint8_t i;
    /* Wants fresh data. */
    encoder_ext_update_step ();
    static const uint8_t reverse[AC_ENCODER_EXT_NB] =
      { AC_ENCODER_EXT_REVERSE };
    /* Update each encoder. */
    for (i = 0; i < AC_ENCODER_EXT_NB; i++)
      {
	/* About shift: this needs to be done after the subtraction to handle
	 * input value under/overflow.  The subtraction is done modulo 2^16
	 * and the result is signed.
	 *
	 * However, there is two problems:
	 *  - shifting rounds towards -infinity, so this is not fair,
	 *  - shifting eliminates small differences, so 1 + 1 + 1 + 1 = 0.
	 * To fix these problems, unused bits must be cleared before the
	 * subtraction is done. */
	uint16_t now = encoder_ext[i].step_acc;
	now &= 0xffff << AC_ENCODER_EXT_SHIFT;
	int16_t diff = (int16_t) (now - encoder_ext[i].old)
	    >> AC_ENCODER_EXT_SHIFT;
	if (reverse[i])
	    diff = -diff;
	encoder_ext[i].encoder->diff = diff;
	encoder_ext[i].encoder->cur += diff;
	encoder_ext[i].old = now;
      }
}

