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
#if PWM1or2 || PWM3or4
    spi_init (SPI_IT_DISABLE | SPI_ENABLE | SPI_MSB_FIRST | SPI_MASTER |
	      SPI_CPOL_FALLING | SPI_CPHA_SETUP | SPI_FOSC_DIV16);
#endif
}

/** Update PWM values. */
void
pwm_mp_update (void)
{
#if PWM1or2 || PWM3or4
    uint8_t v;
    uint8_t cks;
    if (PWM1c (PWM1) || PWM2c (PWM2) || PWM3c (PWM3) || PWM4c (PWM4))
	pwm_mp_go = 1;
    if (!pwm_mp_go)
	return;
#endif
#if PWM1or2
    /* Chip enable. */
    PORTB &= ~_BV (0);
    /* Convert to 12 bits. */
    uint16_t pwm1 = PWM1c (PWM1) << 1;
    uint16_t pwm2 = PWM2c (PWM2) << 1;
    /* Send, computing checksum on the way. */
    cks = 0x42;
    v = ((pwm1 >> 4) & 0xf0) | ((pwm2 >> 8) & 0x0f);
    spi_send (v);
    cks ^= v;
    v = pwm1;
    spi_send (v);
    cks ^= v;
    v = pwm2;
    spi_send (v);
    cks ^= v;
    spi_send (cks);
    /* Chip disable. */
    PORTB |= _BV (0);
#endif
}

