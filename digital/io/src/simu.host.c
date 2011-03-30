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
#include "pwm.h"

#include "modules/utils/utils.h"
#include "modules/host/host.h"
#include "modules/host/mex.h"
#include "modules/adc/adc.h"
#include "modules/path/path.h"
#include "io.h"

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
uint8_t simu_adc_update = 9, simu_adc_update_cpt;
uint8_t simu_pwm_update = 10, simu_pwm_update_cpt;

/** Sampled switches. */
uint8_t simu_switches;

/** PWM registers. */
uint16_t OCR1A;
uint8_t PORTB;

/** Contact registers. */
uint8_t PORTC, PINC;

/** Unused AVR registers. */
uint8_t PORTD, PORTG, DDRC, DDRD, DDRG;

/** Message types. */
uint8_t simu_mex_jack;
uint8_t simu_mex_color;
uint8_t simu_mex_servo;
uint8_t simu_mex_adc;
uint8_t simu_mex_path;
uint8_t simu_mex_pwm;
uint8_t simu_mex_contact;
uint8_t simu_mex_pos_report;

void
handle_contact (void *user, mex_msg_t *msg)
{
    mex_msg_pop (msg, "B", &PINC);
}

/** Initialise simulation. */
void
simu_init (void)
{
    const char *mex_instance;
    mex_node_connect ();
    mex_instance = host_get_instance ("io0", 0);
    simu_mex_jack = mex_node_reservef ("%s:jack", mex_instance);
    simu_mex_color = mex_node_reservef ("%s:color", mex_instance);
    simu_mex_servo = mex_node_reservef ("%s:servo", mex_instance);
    simu_mex_adc = mex_node_reservef ("%s:adc", mex_instance);
    simu_mex_path = mex_node_reservef ("%s:path", mex_instance);
    simu_mex_pwm = mex_node_reservef ("%s:pwm", mex_instance);
    simu_mex_contact = mex_node_reservef ("%s:contact", mex_instance);
    simu_mex_pos_report = mex_node_reservef ("%s:pos-report", mex_instance);
    mex_node_register (simu_mex_contact, handle_contact, 0);
    PINC = 0x3f;
    simu_servo_update_cpt = 1;
    simu_switch_update_cpt = 1;
    simu_adc_update_cpt = 1;
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
	m = mex_msg_new (simu_mex_servo);
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
	m = mex_msg_new (simu_mex_color);
	m = mex_node_request (m);
	mex_msg_pop (m, "B", &r);
	mex_msg_delete (m);
	if (!r)
	    simu_switches |= 1;
	/* Get jack. */
	m = mex_msg_new (simu_mex_jack);
	m = mex_node_request (m);
	mex_msg_pop (m, "B", &r);
	mex_msg_delete (m);
	if (!r)
	    simu_switches |= 2;
      }
    /* Update ADC. */
    if (simu_adc_update && !--simu_adc_update_cpt)
      {
	simu_adc_update_cpt = simu_adc_update;
	m = mex_msg_new (simu_mex_adc);
	m = mex_node_request (m);
	uint8_t i, n;
	n = mex_msg_len (m) / 2;
	assert (n < UTILS_COUNT (adc_values));
	for (i = 0; i < n; i++)
	    mex_msg_pop (m, "H", &adc_values[i]);
	mex_msg_delete (m);
      }
    /* Send PWM. */
    if (simu_pwm_update && !--simu_pwm_update_cpt)
      {
	simu_pwm_update_cpt = simu_pwm_update;
	m = mex_msg_new (simu_mex_pwm);
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
simu_send_path (vect_t *points, uint8_t len,
		struct path_obstacle_t *obstacles, uint8_t obstacles_nb)
{
    int i;
    mex_msg_t *m;
    m = mex_msg_new (simu_mex_path);
    for (i = 0; i < len; i++)
	mex_msg_push (m, "hh", points[i].x, points[i].y);
    mex_node_send (m);
}

void
simu_send_pos_report (vect_t *pos, uint8_t pos_nb, uint8_t id)
{
    mex_msg_t *m;
    m = mex_msg_new (simu_mex_pos_report);
    mex_msg_push (m, "b", id);
    for (; pos_nb; pos++, pos_nb--)
	mex_msg_push (m, "hh", pos->x, pos->y);
    mex_node_send (m);
}

