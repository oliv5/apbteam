#ifndef motor_model_host_h
#define motor_model_host_h
/* motor_model.host.h - DC motor model. */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
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

/** Motor and load characteristics. */
struct motor_model_def_t
{
    /* Motor characteristics. */
    double Ke;	/* Speed constant ((rad/s)/V). */
    double Kt;	/* Torque constant (N.m/A). */
    double Rf;	/* Bearing friction (N.m/(rad/s)). */
    double R;	/* Terminal resistance (Ohm). */
    double L;	/* Terminal inductance (H). */
    /* Gearbox characteristics. */
    double i_G;	/* Gearbox ratio. */
    double ro_G;/* Gearbox efficiency. */
    /* Load characteristics. */
    double J;	/* Load at gearbox output (kg.m^2). */
    /* Hardware limits (use +/-INFINITY for none). */
    double th_min; /* Minimum theta value. */
    double th_max; /* Maximum theta value. */
};
typedef struct motor_model_def_t motor_model_def_t;

/** Motor and load characteristics and current data.  Angular speed and theta
 * are at motor output, not gearbox output. */
struct motor_model_t
{
    /* Motor and load characteristics. */
    struct motor_model_def_t m;
    /* Simulation parameters. */
    double h;	/* Simulation time step (s). */
    int d;	/* Simulation time step division. */
    /* Simulation current state. */
    double t;	/* Current time (not really used) (s). */
    double u;	/* Current input voltage (V). */
    double i;	/* Current current (A). */
    double o;	/* Current angular speed (o for omega) (rad/s). */
    double th;	/* Current theta (th for theta) (rad). */
};
typedef struct motor_model_t motor_model_t;

/** Make a simulation step. */
void motor_model_step (motor_model_t *m);

#endif /* motor_model_host_h */
