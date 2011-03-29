#ifndef encoder_h
#define encoder_h
/* encoder.h */
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
#include "preproc.h"

/** Encoder state. */
struct encoder_t
{
    /** Current position. */
    uint16_t cur;
    /** Difference from last update. */
    int16_t diff;
};
typedef struct encoder_t encoder_t;

#ifdef HOST
# include "encoder_host.h"
# define ENCODER_MODULE host
#else
# if AC_ENCODER_USE_EXT
#  include "ext/encoder_ext.h"
#  define ENCODER_MODULE ext
# endif
#endif

/** Initialize an encoder and attach it to provided encoder structure. */
extern inline void
encoder_init (uint8_t index, encoder_t *encoder)
{
    PREPROC_PASTE (encoder_, ENCODER_MODULE, _init) (index, encoder);
}

/** Update one step.  If encoders are not read fast enough, they could
 * overflow, call this function often for an update step. */
extern inline void
encoder_update_step (void)
{
    PREPROC_PASTE (encoder_, ENCODER_MODULE, _update_step) ();
}

/** Update overall encoder values and compute diffs. */
extern inline void
encoder_update (void)
{
    PREPROC_PASTE (encoder_, ENCODER_MODULE, _update) ();
}

#endif /* encoder_h */
