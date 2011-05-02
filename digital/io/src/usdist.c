/* usdist.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
#include "usdist.h"
#include "timer.h"

#include "modules/adc/adc.h"
#include "modules/utils/utils.h"
#include "io.h"

#include "simu.host.h"

uint16_t usdist_mm[USDIST_NB];

/** Describe a sensor. */
struct usdist_sensor_t
{
    /** ADC number. */
    uint8_t adc;
    /** Sync PORT. */
    volatile uint8_t *sync_port;
    /** Sync DDR. */
    volatile uint8_t *sync_ddr;
    /** Sync bit mask. */
    uint8_t sync_bv;
};

/** Define sensors configuration. */
#define USDIST_SENSOR(adc, p, n) \
    { adc, &IO_PORT_ (p, n), &IO_DDR_ (p, n), IO_BV_ (p, n) }
struct usdist_sensor_t usdist_sensors[USDIST_NB] = {
    USDIST_SENSOR (0, G, 3),
    USDIST_SENSOR (1, G, 1),
    USDIST_SENSOR (2, C, 7),
    USDIST_SENSOR (3, D, 4),
};

void
usdist_init (void)
{
    uint8_t i;
    adc_init ();
    for (i = 0; i < USDIST_NB; i++)
      {
	usdist_mm[i] = 0xffff;
	*usdist_sensors[i].sync_port &= ~usdist_sensors[i].sync_bv;
	*usdist_sensors[i].sync_ddr |= usdist_sensors[i].sync_bv;
      }
}

uint8_t
usdist_update (void)
{
    /* Current measuring sensor. */
    static uint8_t current;
    /* Number of cycles until the measure ends. */
    static uint8_t wait;
    /* Set to one once first cycle is done. */
    static uint8_t init;
    /* Time to measure? */
    if (wait == 0)
      {
	if (init)
	  {
	    /* Stop sensor. */
	    *usdist_sensors[current].sync_port &=
		~usdist_sensors[current].sync_bv;
	    /* Read ADC value. */
	    adc_start (usdist_sensors[current].adc);
	    while (!adc_checkf ())
		;
	    uint16_t v = adc_read ();
	    /* Our sensors return a value between 1 and 5 V proportional to
	     * the distance between calibrated values.  Ignore faulty sensors. */
	    if (v <= 1024 / 5 / 4)
		usdist_mm[current] = 0xffff;
	    else if (v <= 1024 / 5)
		usdist_mm[current] = USDIST_MM_MIN;
	    else
		usdist_mm[current] = USDIST_MM_MIN
		    + ((uint32_t) (v - 1024 / 5)
		       * (USDIST_MM_MAX - USDIST_MM_MIN)
		       / (4 * 1024 / 5));
	    if (usdist_mm[current] >= USDIST_MM_TOO_FAR)
		usdist_mm[current] = 0xffff;
	    /* Next. */
	    current = (current + 1) % USDIST_NB;
	  }
	init = 1;
	/* Prepare next measure. */
	*usdist_sensors[current].sync_port |=
	    usdist_sensors[current].sync_bv;
	wait = USDIST_PERIOD_CYCLE;
	/* New mesure done. */
	return 1;
      }
    else
      {
	wait--;
	return 0;
      }
}

