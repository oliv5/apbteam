#ifndef encoder_corrector_h
#define encoder_corrector_h
/* encoder_corrector.h */
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
#include "encoder.h"

/** Encoder corrector state. */
struct encoder_corrector_t
{
    /** Current uncorrected value. */
    int32_t cur_raw;
    /** Correction factor (f8.24). */
    uint32_t correction;
};
typedef struct encoder_corrector_t encoder_corrector_t;

/** Set correction factor (f8.24). */
extern inline void
encoder_corrector_set_correction (encoder_corrector_t *corrector,
				  uint32_t correction)
{
    corrector->correction = correction;
}

/** Initialise corrector. */
void
encoder_corrector_init (encoder_corrector_t *corrector);

/** Update correction on a single encoder.  Encoder state will be changed. */
void
encoder_corrector_update (encoder_corrector_t *corrector, encoder_t *encoder);

#endif /* encoder_corrector_h */
