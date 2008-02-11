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
#include "io.h"

#include "modules/utils/utils.h"

// Control outputs of H-bridges
// _L_/_R_ : Left or Right bridge
// A/B : bridge leg
// H/L : High or Low side

// _L_AH : PORTC6
#define _L_AH_1 PORTC |= 0x40
#define _L_AH_0 PORTC &= ~0x40
// _L_AL : PORTC7
#define _L_AL_1 PORTC |= 0x80
#define _L_AL_0 PORTC &= ~0x80
// _L_BH : PORTC4
#define _L_BH_1 PORTC |= 0x10
#define _L_BH_0 PORTC &= ~0x10
// _L_BL : PORTC5
#define _L_BL_1 PORTC |= 0x20
#define _L_BL_0 PORTC &= ~0x20


// _R_AH : PORTD6
#define _R_AH_1 PORTD |= 0x40
#define _R_AH_0 PORTD &= ~0x40
// _R_AL : PORTD7
#define _R_AL_1 PORTD |= 0x80
#define _R_AL_0 PORTD &= ~0x80
// _R_BH : PORTC2
#define _R_BH_1 PORTC |= 0x04
#define _R_BH_0 PORTC &= ~0x04
// _R_BL : PORTC3
#define _R_BL_1 PORTC |= 0x08
#define _R_BL_0 PORTC &= ~0x08

// Timer for _L_ and _R_ control
#define TCNT_L_	TCNT0
#define TCNT_R_	TCNT2
#define OCR_L_	OCR0
#define OCR_R_	OCR2
#define TCCR_L_	TCCR0
#define TCCR_R_	TCCR2

// timer configuration
// for 57.21kHz : prescaler = 0 : CSx2:0 = 0x01
// for 7.68kHz  : prescaler = 8 : CSx2:0 = 0x02
#define TCCR_LR_CFG (regv (FOC0, WGM00, COM01, COM00, WGM01, CS02, CS01, CS00, \
			      0,     0,     0,     0,     0,    0,    1,    0))

// timer interrupts configuration
#define TIMSK_LR_CFG (regv (OCIE2, TOIE2, TICIE1, OCIE1A, OCIE1B, TOIE1, OCIE0, TOIE0, \
			        1,     1,      0,      0,      0,      0,     1,     1))

// PWM max, min, and offset values
#define PWM_MIN_LR_ 0x10
#define PWM_MAX_LR_ 0xF0
#define PWM_OFFSET_LR_ 0x00

// Current limitation
#define CURLIM_MAX 0x80
#define OCR_CurLim_L_ OCR1A
#define OCR_CurLim_R_ OCR1B

// functions
void init_timer_LR_(void);
void init_curLim (void);
uint8_t get_curLim_temp (uint8_t temperature);
uint8_t get_curLim_bat (uint8_t battery);
void update_curLim(void);
void launch_envTest(void);
void setCurLim_soft(uint8_t curLim);


#endif /* mp_pwm_LR__h */
