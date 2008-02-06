#ifndef mp_pwm_LR__h
#define mp_pwm_LR__h
/* mp_pwm_LR_.h */
/*  {{{
 *
 * Copyright (C) 2008 Pierre Prot
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

#define OCR_L_ OCR0
#define OCR_R_ OCR2

#define Timer_L_ TOCNT
#define Timer_R_ T2CNT

#define PWM_MIN_LR_ 0x10
#define PWM_MAX_LR_ 0xF0
#define PWM_OFFSET_LR_ 0x00

void init_timer_LR_(void);
void init_curLim (void);
uint8_t get_curLim_temp (uint8_t temperature);
uint8_t get_curLim_bat (uint8_t battery);
void update_curLim(void);
void launch_envTest(void);
void setCurLim_soft(uint8_t curLim);


#endif /* mp_pwm_LR__h */
