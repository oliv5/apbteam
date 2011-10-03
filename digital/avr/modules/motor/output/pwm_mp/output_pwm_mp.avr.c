/* output_pwm_mp.avr.c */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "output_pwm_mp.h"

#include "modules/spi/spi.h"
#include "io.h"

/** Define the maximum PWM value for motor power.
 * This value is lowered until the bug relatives to maximum value is fixed
 * (rounding after shifting bug). */
#define OUTPUT_PWM_MP_MAX 0x3f0

/* Test given configuration. */
#if AC_OUTPUT_PWM_MP_NB != 2 && AC_OUTPUT_PWM_MP_NB != 4
# error "motor/output/pwm_mp: unsupported configuration"
#endif

/* Define test macro. */
#define IF_MP(mp) (AC_OUTPUT_PWM_MP_NB >= ((mp) + 1) * 2)

/** Output information. */
struct output_pwm_mp_t
{
    /** Associated output state. */
    struct output_t *output;
};
typedef struct output_pwm_mp_t output_pwm_mp_t;

/** Global output information. */
output_pwm_mp_t output_pwm_mp[AC_OUTPUT_PWM_MP_NB];

/** Global enable flag. */
uint8_t output_pwm_mp_go;

static void
output_pwm_mp_update_single (int16_t value1, int16_t value2);

/** Initialize hardware, to be done once. */
static void
output_pwm_mp_init_hardware (void)
{
    static uint8_t inited;
    if (!inited)
      {
#if IF_MP (0) && SPI0_DRIVER != SPI_DRIVER_HARD
	/* This is done in spi_init for hardware driver. */
	IO_SET (AC_OUTPUT_PWM_MP_SPI_SS_IO_0);
	IO_OUTPUT (AC_OUTPUT_PWM_MP_SPI_SS_IO_0);
#endif
#if IF_MP (1)
	IO_SET (AC_OUTPUT_PWM_MP_SPI_SS_IO_1);
	IO_OUTPUT (AC_OUTPUT_PWM_MP_SPI_SS_IO_1);
#endif
	/* Initialise SPI. */
	spi_init (SPI_MASTER, SPI_CPOL_FALLING | SPI_CPHA_SETUP,
		  SPI_MSB_FIRST, SPI_FOSC_DIV16);
	/* Reset PWM values at reset. */
#if IF_MP (0)
	IO_CLR (AC_OUTPUT_PWM_MP_SPI_SS_IO_0);
	output_pwm_mp_update_single (0, 0);
	IO_SET (AC_OUTPUT_PWM_MP_SPI_SS_IO_0);
#endif
#if IF_MP (1)
	IO_CLR (AC_OUTPUT_PWM_MP_SPI_SS_IO_1);
	output_pwm_mp_update_single (0, 0);
	IO_SET (AC_OUTPUT_PWM_MP_SPI_SS_IO_1);
#endif
	/* Done. */
	inited = 1;
      }
}

void
output_pwm_mp_init (uint8_t index, output_t *output)
{
    /* Need initialized hardware. */
    output_pwm_mp_init_hardware ();
    /* Keep output structure for future usage. */
    output_pwm_mp[index].output = output;
    /* Reduce maximum output if needed. */
    if (output->max > OUTPUT_PWM_MP_MAX)
	output->max = OUTPUT_PWM_MP_MAX;
}

/** Transmit value to currently selected slave. */
static void
output_pwm_mp_update_single (int16_t value1, int16_t value2)
{
    uint8_t v;
    uint8_t cks;
    /* Convert to 12 bits. */
    int16_t pwm1c = value1 << 1;
    int16_t pwm2c = value2 << 1;
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

void
output_pwm_mp_update (void)
{
    uint8_t i;
    /* Activate output at first non zero value. */
    if (!output_pwm_mp_go)
      {
	for (i = 0; i < AC_OUTPUT_PWM_MP_NB; i++)
	    if (output_pwm_mp[i].output->cur)
		output_pwm_mp_go = 1;
      }
    /* Update each output. */
    if (output_pwm_mp_go)
      {
#if IF_MP (0)
	/* Slave select. */
	IO_CLR (AC_OUTPUT_PWM_MP_SPI_SS_IO_0);
	/* Transmit values. */
	output_pwm_mp_update_single (output_pwm_mp[0].output->cur,
				     output_pwm_mp[1].output->cur);
	/* Slave deselect. */
	IO_SET (AC_OUTPUT_PWM_MP_SPI_SS_IO_0);
#endif
#if IF_MP (1)
	/* Slave select. */
	IO_CLR (AC_OUTPUT_PWM_MP_SPI_SS_IO_1);
	/* Transmit values. */
	output_pwm_mp_update_single (output_pwm_mp[2].output->cur,
				     output_pwm_mp[3].output->cur);
	/* Slave deselect. */
	IO_SET (AC_OUTPUT_PWM_MP_SPI_SS_IO_1);
#endif
      }
}

