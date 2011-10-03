/* output.c */
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
#include "output.h"

#ifdef HOST
# include "output_host.h"
# define OUTPUT_MODULES ,host
#else
# if AC_OUTPUT_USE_PWM_OCR
#  include "pwm_ocr/output_pwm_ocr.h"
#  define OUTPUT_MODULE_1 ,pwm_ocr
# else
#  define OUTPUT_MODULE_1
# endif
# if AC_OUTPUT_USE_PWM_MP
#  include "pwm_mp/output_pwm_mp.h"
#  define OUTPUT_MODULE_2 ,pwm_mp
# else
#  define OUTPUT_MODULE_2
# endif
# define OUTPUT_MODULES \
    OUTPUT_MODULE_1 OUTPUT_MODULE_2
#endif

void
output_init (uint8_t index, output_t *output)
{
    /* Initialize corresponding module. */
#ifdef HOST
    output_host_init (index, output);
#else
# define output_init__(module, module_index) \
    PREPROC_PASTE (output_, module, _init) (module_index, output)
# define output_init_(index, output_info) \
    case index: output_init__ output_info; break;
    switch (index)
      {
	PREPROC_FOR_ENUM (output_init_, AC_OUTPUT_LIST)
      }
#endif
}

void
output_set (output_t *output, int16_t value)
{
    /* Reverse output if requested. */
    if (output->reverse)
	value = -value;
    /* Saturation and dead zone. */
    if (value > output->max)
	output->cur = output->max;
    else if (value < -output->max)
	output->cur = -output->max;
    else if (value > -output->min && value < output->min)
	output->cur = 0;
    else
	output->cur = value;
}

void
output_update (void)
{
    /* Update each module. */
#define output_update_(module) PREPROC_PASTE (output_, module, _update) ();
    PREPROC_FOR (output_update_ OUTPUT_MODULES)
}

