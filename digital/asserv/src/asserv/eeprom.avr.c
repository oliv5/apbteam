/* eeprom.avr.c - Save parameters to internal EEPROM. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
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
#include "eeprom.h"

#include "modules/utils/utils.h"
#include "modules/utils/crc.h"

#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "cs.h"
#include "postrack.h"
#include "traj.h"

#define EEPROM_INDEX_NB (2 + AC_ASSERV_AUX_NB)

/* WARNING:
 * If you change EEPROM format, be sure to change the EEPROM_KEY in header if
 * your new format is not compatible with the old one or you may load
 * garbages in parameters. */
struct eeprom_t
{
    /** Identify parameters version. */
    uint8_t key;
    /** Saved parameters. */
    uint32_t encoder_right_correction;
    struct {
	uint8_t max;
	uint8_t slow;
	uint16_t acc;
    } speed[EEPROM_INDEX_NB];
    struct {
	uint16_t kp;
	uint16_t ki;
	uint16_t kd;
	uint16_t e_sat;
	uint16_t i_sat;
	uint16_t d_sat;
    } pos[EEPROM_INDEX_NB];
    struct {
	uint16_t error_limit;
	uint16_t speed_limit;
	uint8_t counter_limit;
    } bd[EEPROM_INDEX_NB];
    struct {
	uint8_t reverse;
    } output[EEPROM_INDEX_NB];
    uint16_t postrack_footing;
    uint16_t traj_eps;
    uint16_t traj_aeps;
    uint16_t traj_angle_limit;
    /** CRC of the whole structure. */
    uint8_t crc;
};

/* If EEPROM_DEFAULTS is defined, it should be the name of a file to include
 * which defines the structure eeprom_defaults in PROGMEM and
 * EEPROM_DEFAULTS_KEY which should match EEPROM_KEY.  Those defaults values
 * are used when no EEPROM set is good. */
#ifdef EEPROM_DEFAULTS
# include EEPROM_DEFAULTS
# if EEPROM_DEFAULTS_KEY != EEPROM_KEY
#  error "EEPROM defaults are not compatible (key mismatch)"
# endif
#endif

EEMEM struct eeprom_t eeprom_params[4];

/** Index of loaded eeprom block. */
int8_t eeprom_loaded;

static void
eeprom_read_params_helper (struct eeprom_t *loaded, uint8_t index,
			   speed_control_t *speed, pos_control_t *pos,
			   blocking_detection_t *bd, output_t *output)
{
    speed->max = loaded->speed[index].max;
    speed->slow = loaded->speed[index].slow;
    speed->acc = loaded->speed[index].acc;
    pos->kp = loaded->pos[index].kp;
    pos->ki = loaded->pos[index].ki;
    pos->kd = loaded->pos[index].kd;
    pos->e_sat = loaded->pos[index].e_sat;
    pos->i_sat = loaded->pos[index].i_sat;
    pos->d_sat = loaded->pos[index].d_sat;
    bd->error_limit = loaded->bd[index].error_limit;
    bd->speed_limit = loaded->bd[index].speed_limit;
    bd->counter_limit = loaded->bd[index].counter_limit;
    output_set_reverse (output, loaded->output[index].reverse);
}

/* Read parameters from eeprom. */
void
eeprom_read_params (void)
{
    uint8_t i;
    struct eeprom_t loaded;
    eeprom_loaded = -1;
    /* Load first good set. */
    for (i = 0; i < UTILS_COUNT (eeprom_params); i++)
      {
	/* Read EEPROM. */
	eeprom_read_block (&loaded, &eeprom_params[i],
			   sizeof (struct eeprom_t));
	/* Check CRC. */
	if (loaded.key == EEPROM_KEY
	    && crc_compute ((uint8_t *) &loaded,
			    sizeof (struct eeprom_t)) == 0)
	  {
	    /* Ok. */
	    eeprom_loaded = i;
	    break;
	  }
      }
    /* Load defaults if no set is good. */
#ifdef EEPROM_DEFAULTS
    if (eeprom_loaded == -1)
      {
	memcpy_P (&loaded, &eeprom_defaults, sizeof (struct eeprom_t));
	eeprom_loaded = 0xDF; /* DeFaults. */
      }
#endif
    if (eeprom_loaded != -1)
      {
	/* Ok, load parameters. */
	encoder_corrector_set_correction (&encoder_right_corrector,
					  loaded.encoder_right_correction);
	eeprom_read_params_helper (&loaded, 0, &cs_main.speed_theta,
				   &cs_main.pos_theta,
				   &cs_main.blocking_detection_theta,
				   &output_left);
	eeprom_read_params_helper (&loaded, 1, &cs_main.speed_alpha,
				   &cs_main.pos_alpha,
				   &cs_main.blocking_detection_alpha,
				   &output_right);
	for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	    eeprom_read_params_helper (&loaded, 2 + i, &cs_aux[i].speed,
				       &cs_aux[i].pos,
				       &cs_aux[i].blocking_detection,
				       &output_aux[i]);
	postrack_set_footing (loaded.postrack_footing);
	traj_eps = loaded.traj_eps;
	traj_aeps = loaded.traj_aeps;
	traj_set_angle_limit (loaded.traj_angle_limit);
      }
}

static void
eeprom_write_params_helper (struct eeprom_t *param, uint8_t index,
			    speed_control_t *speed, pos_control_t *pos,
			    blocking_detection_t *bd, output_t *output)
{
    param->speed[index].max = speed->max;
    param->speed[index].slow = speed->slow;
    param->speed[index].acc = speed->acc;
    param->pos[index].kp = pos->kp;
    param->pos[index].ki = pos->ki;
    param->pos[index].kd = pos->kd;
    param->pos[index].e_sat = pos->e_sat;
    param->pos[index].i_sat = pos->i_sat;
    param->pos[index].d_sat = pos->d_sat;
    param->bd[index].error_limit = bd->error_limit;
    param->bd[index].speed_limit = bd->speed_limit;
    param->bd[index].counter_limit = bd->counter_limit;
    param->output[index].reverse = output->reverse;
}

/* Write parameters to eeprom. */
void
eeprom_write_params (void)
{
    uint8_t i;
    struct eeprom_t p;
    /* Prepare parameters. */
    p.key = EEPROM_KEY;
    p.encoder_right_correction = encoder_right_corrector.correction;
    eeprom_write_params_helper (&p, 0, &cs_main.speed_theta,
				&cs_main.pos_theta,
				&cs_main.blocking_detection_theta,
				&output_left);
    eeprom_write_params_helper (&p, 1, &cs_main.speed_alpha,
				&cs_main.pos_alpha,
				&cs_main.blocking_detection_alpha,
				&output_right);
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	eeprom_write_params_helper (&p, 2 + i, &cs_aux[i].speed,
				    &cs_aux[i].pos,
				    &cs_aux[i].blocking_detection,
				    &output_aux[i]);
    p.postrack_footing = postrack_footing;
    p.traj_eps = traj_eps;
    p.traj_aeps = traj_aeps;
    p.traj_angle_limit = traj_angle_limit;
    p.crc = crc_compute ((uint8_t *) &p, sizeof (p) - 1);
    /* Write every sets. */
    for (i = 0; i < UTILS_COUNT (eeprom_params); i++)
      {
	eeprom_write_block (&p, &eeprom_params[i],
			    sizeof (struct eeprom_t));
      }
}

/* Clear eeprom parameters. */
void
eeprom_clear_params (void)
{
    uint8_t i;
    /* Clear every sets. */
    for (i = 0; i < UTILS_COUNT (eeprom_params); i++)
	eeprom_write_byte (&eeprom_params[i].key, 0);
}

