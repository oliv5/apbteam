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

#include "modules/adc/adc.h"
#include "modules/utils/utils.h"
#include "io.h"

uint16_t usdist_mm[USDIST_NB];

void
usdist_init (void)
{
    adc_init ();
}

void
usdist_update (void)
{
    uint8_t i;
    /* Simple algorithm, just make a conversion for every sensor.  This should
     * be improved because it takes too long. */
    for (i = 0; i < USDIST_NB; i++)
      {
	adc_start (i);
	while (!adc_checkf ())
	    ;
	uint16_t v = adc_read ();
	/* Our sensors return a value between 1 and 5 V proportional to the
	 * distance between calibrated values. */
	if (v <= 1024 / 5)
	    usdist_mm[i] = USDIST_MM_MIN;
	else
	    usdist_mm[i] = USDIST_MM_MIN
		+ (uint32_t) (v - 1024 / 5) * (USDIST_MM_MAX - USDIST_MM_MIN)
		/ (4 * 1024 / 5);
	if (usdist_mm[i] >= USDIST_MM_TOO_FAR)
	    usdist_mm[i] = 0xffff;
      }
}

