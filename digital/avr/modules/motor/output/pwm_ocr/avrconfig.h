#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h - motor/output/pwm_ocr configuration template. */
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

/* Example from mimot board. */

/* motor/output/pwm_ocr - Output Compare PWM output module. */
/** For each output, define output parameters:
 *
 * (timer, ocr, pwm_io, dir_io[, brake_io])
 *
 * With:
 *  - timer: timer number (ex: 1 for TIMER1)
 *  - ocr: output compare (ex: A for output compare A)
 *  - mode: compare output mode (ex: 2, see datasheet)
 *  - pwm_io: corresponding io port (ex: B, 1)
 *  - dir_io: io port used for direction (ex: B, 2)
 *  - brake_io: optional io port used for brake (ex: B, 3)
 */
#define AC_OUTPUT_PWM_OCR_LIST \
    (1, A, 2, D,5, D,3, A,4), \
    (1, B, 2, D,4, D,2, A,5)
/** Clock select for each used timer. */
#define AC_OUTPUT_PWM_OCR_CS_1 0b0001
/** Waveform Generation Mode for each used timer. */
#define AC_OUTPUT_PWM_OCR_WGM_1 0b0011
/** Offset added to PWM value to compensate for H-bridge weakness. */
#define AC_OUTPUT_PWM_OCR_OFFSET 0x40

#endif /* avrconfig_h */
