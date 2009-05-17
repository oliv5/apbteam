/* simu.host.c - Host simulation. */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "simu.host.h"

#include "servo.h"
#include "sharp.h"
#include "pwm.h"

#include "modules/utils/utils.h"
#include "modules/host/host.h"
#include "modules/host/mex.h"
#include "modules/adc/adc.h"
#include "modules/path/path.h"
#include "io.h"

enum
{
    MSG_SIMU_IO_JACK = 0xb0,
    MSG_SIMU_IO_COLOR = 0xb1,
    MSG_SIMU_IO_SERVO = 0xb2,
    MSG_SIMU_IO_SHARPS = 0xb3,
    MSG_SIMU_IO_PATH = 0xb4,
    MSG_SIMU_IO_PWM = 0xb5,
    MSG_SIMU_IO_CONTACT = 0xb6,
};

/** Requested servo position. */
uint8_t servo_high_time_[SERVO_NUMBER];

/** Current servo position. */
uint8_t servo_high_time_current_[SERVO_NUMBER];

/** Servo speed is about 120 ms for 60 degrees.  This means about 360 ms for
 * the full swing. */
#define SERVO_SPEED (int) ((256 / (360.0 / 4.4444)) + 0.5)

/** Do not update too often, interface is too slow. */
uint8_t simu_servo_update = 10, simu_servo_update_cpt;
uint8_t simu_switch_update = 100, simu_switch_update_cpt;
uint8_t simu_sharps_update = 9, simu_sharps_update_cpt;
uint8_t simu_pwm_update = 10, simu_pwm_update_cpt;

/** Sampled switches. */
uint8_t simu_switches;

/** PWM registers. */
uint16_t OCR1A;
uint8_t PORTB;

/** Contact registers. */
uint8_t PORTC, PINC;

void
handle_contact (void *user, mex_msg_t *msg)
{
    mex_msg_pop (msg, "B", &PINC);
}

/** Initialise simulation. */
void
simu_init (void)
{
    mex_node_connect ();
    mex_node_register (MSG_SIMU_IO_CONTACT, handle_contact, 0);
    PINC = 0x3f;
    simu_servo_update_cpt = 1;
    simu_switch_update_cpt = 1;
    simu_sharps_update_cpt = 1;
    simu_pwm_update_cpt = 1;
}

/** Make a simulation step. */
void
simu_step (void)
{
    int i;
    mex_msg_t *m;
    /* Update servos. */
    for (i = 0; i < SERVO_NUMBER; i++)
      {
	if (UTILS_ABS (servo_high_time_current_[i] - servo_high_time_[i]) <
	    SERVO_SPEED)
	    servo_high_time_current_[i] = servo_high_time_[i];
	else if (servo_high_time_current_[i] < servo_high_time_[i])
	    servo_high_time_current_[i] += SERVO_SPEED;
	else
	    servo_high_time_current_[i] -= SERVO_SPEED;
      }
    /* Send servos. */
    if (simu_servo_update && !--simu_servo_update_cpt)
      {
	simu_servo_update_cpt = simu_servo_update;
	m = mex_msg_new (MSG_SIMU_IO_SERVO);
	for (i = 0; i < SERVO_NUMBER; i++)
	    mex_msg_push (m, "B", servo_high_time_current_[i]);
	mex_node_send (m);
      }
    /* Update switches. */
    if (simu_switch_update && !--simu_switch_update_cpt)
      {
	simu_switch_update_cpt = simu_switch_update;
	uint8_t r;
	simu_switches = 0;
	/* Get color switch. */
	m = mex_msg_new (MSG_SIMU_IO_COLOR);
	m = mex_node_request (m);
	mex_msg_pop (m, "B", &r);
	mex_msg_delete (m);
	if (!r)
	    simu_switches |= 1;
	/* Get jack. */
	m = mex_msg_new (MSG_SIMU_IO_JACK);
	m = mex_node_request (m);
	mex_msg_pop (m, "B", &r);
	mex_msg_delete (m);
	if (!r)
	    simu_switches |= 2;
      }
    /* Update sharps. */
    if (simu_sharps_update && !--simu_sharps_update_cpt)
      {
	simu_sharps_update_cpt = simu_sharps_update;
	m = mex_msg_new (MSG_SIMU_IO_SHARPS);
	m = mex_node_request (m);
	uint8_t i;
	for (i = 0; i < SHARP_NUMBER; i++)
	    mex_msg_pop (m, "H", &adc_values[i]);
	mex_msg_delete (m);
      }
    /* Send PWM. */
    if (simu_pwm_update && !--simu_pwm_update_cpt)
      {
	simu_pwm_update_cpt = simu_pwm_update;
	m = mex_msg_new (MSG_SIMU_IO_PWM);
	mex_msg_push (m, "h", (IO_PORT (PWM_DIR_IO) & IO_BV (PWM_DIR_IO))
		      ? PWM_OCR : -PWM_OCR);
	mex_node_send (m);
      }
}

void
servo_init (void)
{
}

void
servo_set_high_time (uint8_t servo, uint8_t high_time)
{
    servo_high_time_[servo] = high_time;
}

uint8_t
servo_get_high_time (uint8_t servo)
{
    return servo_high_time_[servo];
}

void
switch_init (void)
{
}

uint8_t
switch_get_color (void)
{
    return (simu_switches & 1) ? 1 : 0;
}

uint8_t
switch_get_jack (void)
{
    return (simu_switches & 2) ? 1 : 0;
}

void
switch_update (void)
{
}

void
main_timer_init (void)
{
    simu_init ();
}

uint8_t
main_timer_wait (void)
{
    mex_node_wait_date (mex_node_date () + 4);
    simu_step ();
    return 0;
}

void
eeprom_load_param (void)
{
}

void
eeprom_save_param (void)
{
}

void
eeprom_clear_param (void)
{
}

/** Send computed path. */
void
simu_send_path (uint16_t *points, uint8_t len,
		struct path_obstacle_t *obstacles, uint8_t obstacles_nb)
{
    int i;
    mex_msg_t *m;
    m = mex_msg_new (MSG_SIMU_IO_PATH);
    for (i = 0; i < len; i++)
	mex_msg_push (m, "h", points[i]);
    mex_node_send (m);
}

