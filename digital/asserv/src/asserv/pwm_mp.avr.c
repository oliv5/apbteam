/* pwm_mp.avr.c - PWM to be used with motor-power-avr. */
/* asserv - Position & speed motor control on AVR. {{{
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
#include "pwm_mp.avr.h"
#include "pwm.h"

#include "modules/spi/spi.h"
#include "modules/utils/utils.h"
#include "io.h"

/** Assign PWM outputs. */
#define PWM1 pwm_left
#define PWM2 pwm_right
#undef PWM3
#undef PWM4

#include "pwm_config.h"

/** When true, start transmission. */
uint8_t pwm_mp_go;

/** Initialise communication with motor-power-avr. */
void
pwm_mp_init (void)
{
#if PWM3or4
    /* Only needed for PWM3 and 4 because it is done in spi_init for SS (port
     * B0). */
    PORTD |= _BV (4);
    DDRD |= _BV (4);
#endif
#if PWM1or2 || PWM3or4
    spi_init (SPI_IT_DISABLE | SPI_ENABLE | SPI_MSB_FIRST | SPI_MASTER |
	      SPI_CPOL_FALLING | SPI_CPHA_SETUP | SPI_FOSC_DIV16);
#endif
    /* Reset PWM values at reset. */
    pwm_mp_go = 1;
    pwm_mp_update ();
    pwm_mp_go = 0;
}

#if PWM1or2 || PWM3or4

/** Send command using SPI. */
static void
pwm_mp_send (int16_t pwm1, int16_t pwm2)
{
    uint8_t v;
    uint8_t cks;
    /* Convert to 12 bits. */
    int16_t pwm1c = pwm1 << 1;
    int16_t pwm2c = pwm2 << 1;
    if (UTILS_ABS (pwm1c) < 0x20)
	pwm1c = 0;
    if (UTILS_ABS (pwm2c) < 0x20)
	pwm2c = 0;
    /* Send, computing checksum on the way. */
    cks = 0x42;
    v = ((pwm1c >> 4) & 0xf0) | ((pwm2c >> 8) & 0x0f);
    spi_send (v);
    cks ^= v;
    v = pwm1c;
    spi_send (v);
    cks ^= v;
    v = pwm2c;
    spi_send (v);
    cks ^= v;
    spi_send (cks);
}

#endif /* PWM1or2 || PWM3or4 */

/** Update PWM values. */
void
pwm_mp_update (void)
{
#if PWM1or2 || PWM3or4
    if (PWM1c (PWM1) || PWM2c (PWM2) || PWM3c (PWM3) || PWM4c (PWM4))
	pwm_mp_go = 1;
    if (!pwm_mp_go)
	return;
#endif
#if PWM1or2
    /* Chip enable. */
    PORTB &= ~_BV (0);
    pwm_mp_send (PWM1c (PWM1), PWM2c (PWM2));
    /* Chip disable. */
    PORTB |= _BV (0);
#endif
#if PWM3or4
    /* Chip enable. */
    PORTE &= ~_BV (4);
    pwm_mp_send (PWM3c (PWM3), PWM4c (PWM4));
    /* Chip disable. */
    PORTE |= _BV (4);
#endif
}

