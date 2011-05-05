/* pwm.host.c */
/* io-hub - Modular Input/Output. {{{
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
#include "common.h"
#include "pwm.h"

#include "modules/utils/utils.h"
#include "modules/host/host.h"
#include "modules/host/mex.h"

/** PWM context. */
struct pwm_t
{
    /** Current value. */
    int16_t value;
    /** Remaining time before timer elapse, 0 for disabled. */
    uint16_t time;
    /** Value to be used after timer elapse. */
    int16_t value_rest;
};

/** Array of PWM contexts. */
#define PWM(timer, pwm, pwm_io_port, pwm_io_n, dir_io_port, dir_io_n) \
    { 0, 0, 0 },
struct pwm_t pwm[] = {
    AC_IOHUB_PWM
};
#undef PWM

/** Message type. */
uint8_t pwm_mtype;

/** Message has to be sent. */
uint8_t pwm_changed;

void
pwm_init (void)
{
    const char *mex_instance = host_get_instance ("io-hub0", 0);
    pwm_mtype = mex_node_reservef ("%s:pwm", mex_instance);
}

void
pwm_update (void)
{
    uint8_t i;
    /* Update timed PWM. */
    for (i = 0; i < UTILS_COUNT (pwm); i++)
      {
	if (pwm[i].time)
	  {
	    pwm[i].time--;
	    if (pwm[i].time == 0)
		pwm_set (i, pwm[i].value_rest);
	  }
      }
    /* Send change if needed. */
    if (pwm_changed)
      {
	mex_msg_t *m = mex_msg_new (pwm_mtype);
	for (i = 0; i < UTILS_COUNT (pwm); i++)
	    mex_msg_push (m, "h", pwm[i].value);
	mex_node_send (m);
	pwm_changed = 0;
      }
}

void
pwm_set (uint8_t index, int16_t value)
{
    assert (index < UTILS_COUNT (pwm));
    assert (value <= PWM_MAX && value >= -PWM_MAX);
    pwm[index].time = 0;
    pwm[index].value = value;
    pwm_changed = 1;
}

void
pwm_set_timed (uint8_t index, int16_t value, uint16_t time, int16_t value_rest)
{
    assert (index < UTILS_COUNT (pwm));
    assert (value <= PWM_MAX && value >= -PWM_MAX);
    assert (value_rest <= PWM_MAX && value_rest >= -PWM_MAX);
    pwm_set (index, value);
    pwm[index].time = time;
    pwm[index].value_rest = value_rest;
}

