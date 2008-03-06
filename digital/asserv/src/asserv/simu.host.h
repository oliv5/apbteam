#ifndef simu_host_h
#define simu_host_h
/* simu.host.h */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
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
 * Contact :
 *        Web: http://perso.efrei.fr/~schodet/
 * }}} */

/** Simulate some AVR regs. */
extern uint8_t DDRF, PORTC, PORTD, PORTE, PORTF, PORTG, PINC;

/** Overall counter values. */
extern uint16_t counter_left, counter_right;
/** Counter differences since last update.
 * Maximum of 9 significant bits, sign included. */
extern int16_t counter_left_diff, counter_right_diff;

/** Define the direction output for left motor. */
#define PWM_LEFT_DIR 4
/** Define the direction output for right motor. */
#define PWM_RIGHT_DIR 5

/** Define the absolute maximum PWM value. */
#define PWM_MAX 0x3ff

/** PWM values, this is an error if absolute value is greater than the
 * maximum. */
extern int16_t pwm_left, pwm_right;
/** PWM reverse direction, only set pwm dir bits or you will get weird results
 * on port B. */
extern uint8_t pwm_dir;

/** Computed simulated position. */
extern double simu_pos_x, simu_pos_y, simu_pos_a;

#define EEPROM_KEY 0xa5
#define eeprom_read_params() do { } while (0)
#define eeprom_write_params() do { } while (0)
#define eeprom_clear_params() do { } while (0)

/* +AutoDec */

/** Initialise the timer. */
void
timer_init (void);

/** Wait for timer overflow. */
void
timer_wait (void);

/** Read timer value. Used for performance analysis. */
uint8_t
timer_read (void);

/** Initialize the counters. */
void
counter_init (void);

/** Update overall counter values and compute diffs. */
void
counter_update (void);

/** Initialise PWM generator. */
void
pwm_init (void);

/** Update the hardware PWM values. */
void
pwm_update (void);

/* -AutoDec */

#endif /* simu_host_h */
