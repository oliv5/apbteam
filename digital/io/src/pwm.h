#ifndef pwm_h
#define pwm_h
/* pwm.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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

/** PWM maximum value. */
#define PWM_MAX 0x3ff

/** OCR register. */
#define PWM_OCR OCR1A
/** PWM io. */
#define PWM_IO B, 5
/** PWM direction io. */
#define PWM_DIR_IO B, 4

void
pwm_init (void);
void
pwm_set (int16_t value, uint16_t timer);
void
pwm_update (void);

#endif /* pwm_h */
