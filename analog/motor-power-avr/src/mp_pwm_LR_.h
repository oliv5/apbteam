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

// _L_LED0 : PORTB0
#define _L_LED0_1 PORTB |= 0x01
#define _L_LED0_0 PORTB &= ~0x01

// _L_LED1 : PORTB1
#define _L_LED1_1 PORTB |= 0x02
#define _L_LED1_0 PORTB &= ~0x02

// _R_LED0 : PORTB3
#define _R_LED0_1 PORTB |= 0x04
#define _R_LED0_0 PORTB &= ~0x04

// _R_LED1 : PORTB4
#define _R_LED1_1 PORTB |= 0x08
#define _R_LED1_0 PORTB &= ~0x08

#define _L_ACTIVATE_OUTPUTS \
    { \
        DDRC |= 0xf0; /* PWM outputs */ \
        DDRB |= 0x03; /* LED outputs */ \
    }

#define _R_ACTIVATE_OUTPUTS \
    { \
        DDRC |= 0x0c; /* PWM outputs */ \
        DDRD |= 0xc0; \
        DDRB |= 0x0c; /* LED outputs */ \
    }

// Timer for _L_ and _R_ control
#define TCNT_L_	TCNT0
#define TCNT_R_	TCNT2
#define OCR_L_	OCR0
#define OCR_R_	OCR2
#define TCCR_L_	TCCR0
#define TCCR_R_	TCCR2

/** Timer configuration for left and right side
 * for 57.21kHz : prescaler = 0 : CSx2:0 = 0x01
 * for 7.68kHz  : prescaler = 8 : CSx2:0 = 0x02 */
#define TCCR_LR_CFG (regv (FOC0, WGM00, COM01, COM00, WGM01, CS02, CS01, CS00, \
                              0,     1,     0,     0,     1,    0,    1,    0))

/** Set timer interrupts configuration */
#define TIMSK_LR_CFG (regv (OCIE2, TOIE2, TICIE1, OCIE1A, OCIE1B, TOIE1, OCIE0, TOIE0, \
                                1,     1,      0,      0,      0,     0,     1,     1))

/** Defines timer control register for current limitation PWM (both side)
 * toggle OC1x on compare, fast 8bit PWM mode, no prescaling */
#define TCCRA_LR_CFG (regv (COM1A1, COM1A0, COM1B1, COM1B0, FOC1A, FOC1B, WGM11, WGM10, \
                                 1,      0,      1,      0,     0,     0,     0,     1))
#define TCCRB_LR_CFG (regv (ICNC1, ICES1,  5, WGM13, WGM12, CS12, CS11, CS10, \
                                0,     0,  0,     0,     1,    0,    0,    1))
                
/** Defines external interrupts level configuration : 
 *  falling edge of INT0 and INT1 generates an interrup request */
#define MCUCR_LR_CFG (regv (SM2, SE, SM1, SM0, ISC11, ISC10, ISC01, ISC00, \
                              0,  0,   0,   0,     1,     0,     1,     0))

/** Enable external interrupts INT1 and INT0 fir current limitation */
#define GICR_LR_CFG (regv (INT1, INT0, INT2,  4,  3,  2, IVSEL, IVCE, \
                              1,    1,    0,  0,  0,  0,     0,    0))

// PWM max, min, and offset values
#define PWM_MIN_LR_ 0x01
#define PWM_MAX_LR_ 0xF0
#define PWM_OFFSET_LR_ 0x00

// Current limitation
#define CURLIM_MAX 0x80
#define OCR_CurLim_L_ OCR1A
#define OCR_CurLim_R_ OCR1B
#define ILIM_R_io (PIND & (1 << 3))
#define ILIM_L_io (PIND & (1 << 2))

// Vectors
#define ILIM_R_vect INT1_vect
#define ILIM_L_vect INT0_vect
#define OVF_L_vect  TIMER0_OVF_vect
#define OVF_R_vect  TIMER2_OVF_vect
#define COMP_L_vect TIMER0_COMP_vect
#define COMP_R_vect TIMER2_COMP_vect

// Command states
#define CMD_STATE_DIR_0   0x00
#define CMD_STATE_DIR_1   0x01
#define CMD_STATE_BRAKE   0x02
#define CMD_STATE_HIGH_Z  0x03

// functions
void init_timer_LR_(void);
void init_curLim (void);
uint8_t get_curLim_temp (uint8_t temperature);
uint8_t get_curLim_bat (uint8_t battery);
inline void update_curLim(void);
void launch_envTest(void);
void setCurLim_soft(uint8_t curLim);


#endif /* mp_pwm_LR__h */
