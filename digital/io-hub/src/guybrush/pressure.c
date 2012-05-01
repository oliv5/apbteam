/* pressure.c */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "pressure.h"

#include "modules/adc/adc.h"

#include "modules/utils/utils.h"

#include "output.h"
#include "mimot.h"

/** Maximum pressure. */
#define PRESSURE_MAX 1000

/** Hysteresis margin. */
#define PRESSURE_MARGIN (PRESSURE_MAX / 16)

/** Pressure sensor connected ADC. */
#define PRESSURE_SENSOR_ADC 4

/** Pressure FSM states. */
enum
{
    /** Start state, circuit is closed, no pressure. */
    PRESSURE_INITIAL,
    /** Idle, pressure is OK, circuit is open. */
    PRESSURE_IDLE,
    /** Starting the pump, with open circuit. */
    PRESSURE_STARTING,
    /** Pumping until the pressure is OK, circuit is closed. */
    PRESSURE_PUMPING,
};

/** Global context. */
struct pressure_t
{
    /** Pressure sensor threshold values (initialized to 0). */
    uint16_t low, high;
    /** Pressure FSM current state. */
    uint8_t state;
    /** Wait counter before the next action. */
    uint8_t wait;
};
static struct pressure_t pressure;

/** Interval at which the sensor is read. */
#define PRESSURE_INTERVAL 25

/** Rest duration after a pumping session. */
#define PRESSURE_REST 250

/** Wait duration before the pump is started. */
#define PRESSURE_OPEN_WAIT 125

/** Wait duration after the pump has been started before circuit is closed. */
#define PRESSURE_START_WAIT 50

void
pressure_update (void)
{
    if (pressure.wait)
      {
	pressure.wait--;
      }
    else
      {
	switch (pressure.state)
	  {
	  case PRESSURE_INITIAL:
	    if (pressure.low)
	      {
		/* Open circuit. */
		IO_SET (OUTPUT_PNEUM_OPEN);
		pressure.state++;
		pressure.wait = PRESSURE_OPEN_WAIT;
	      }
	    break;
	  case PRESSURE_IDLE:
	    if (pressure_get () < pressure.low)
	      {
		/* Start pump. */
		mimot_motor_output_set (1, 0x3ff);
		pressure.state++;
		pressure.wait = PRESSURE_START_WAIT;
	      }
	    else
		pressure.wait = PRESSURE_INTERVAL;
	    break;
	  case PRESSURE_STARTING:
	    /* Started, close circuit. */
	    IO_CLR (OUTPUT_PNEUM_OPEN);
	    pressure.state++;
	    break;
	  case PRESSURE_PUMPING:
	    /* Pump until high. */
	    if (pressure_get () > pressure.high)
	      {
		/* Done. Stop. Open circuit. */
		mimot_motor1_free ();
		IO_SET (OUTPUT_PNEUM_OPEN);
		pressure.state = PRESSURE_IDLE;
		pressure.wait = PRESSURE_REST;
	      }
	    else
		pressure.wait = PRESSURE_INTERVAL;
	    break;
	  }
      }
}

void
pressure_set (uint16_t low)
{
    low = UTILS_MIN (low, PRESSURE_MAX - PRESSURE_MARGIN);
    pressure.low = low;
    pressure.high = low + PRESSURE_MARGIN;
}

uint16_t
pressure_get (void)
{
    adc_start (PRESSURE_SENSOR_ADC);
    while (!adc_checkf ())
	;
    return adc_read ();
}

