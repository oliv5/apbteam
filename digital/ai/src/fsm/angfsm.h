/*
   AngFSM - Almost Non Generated Finite State Machine
   Copyright 2011-2013 Jerome Jutteau

 This file is part of AngFSM.

 AngFSM is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AngFSM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with AngFSM. If not, see <http://www.gnu.org/licenses/>.
 */

#include <inttypes.h>

#ifndef _ANGFSM_GEN_
#define _ANGFSM_GEN_

#ifndef ANGFSM_NAME
#define ANGFSM_NAME ANGFSM
#endif

/* Usefull macros. */
#define XSTR(s) STR(s)
#define STR(s) #s
#define ANGFSM_PASTE(a, b) a ## b
#define ANGFSM_PASTE_EXPAND(a, b) ANGFSM_PASTE (a, b)
#define ANGFSM_PASTE3_EXPAND(a, b, c)\
ANGFSM_PASTE_EXPAND (a, ANGFSM_PASTE_EXPAND (b, c))
#define FIRST(first, others...) first

#define IFELSE_ARG1(yes, no, params...) \
    IFELSE_ARG1_ (params, IFELSE_ARG1_SEQ_(yes, no))
#define IFELSE_ARG1_(params...) \
    IFELSE_ARG1__ (params)
#define IFELSE_ARG1__( _a1, \
    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, \
    _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
    selector, ...) selector
#define IFELSE_ARG1_SEQ_(yes, no) \
    no, no, no, no, no, no, no, no, no, no, \
    no, no, no, no, no, no, no, no, no, no, \
    yes

#ifdef __HOST_ARCH__
/* Include generated header. */
#include "angfsm_generic.h"
#include XSTR (ANGFSM_PASTE_EXPAND (angfsm_gen_host_, ANGFSM_NAME).h)

#elif defined(__AVR_ARCH__)
/* Include generated header. */
#include "angfsm_generic.h"
#include XSTR (ANGFSM_PASTE_EXPAND (angfsm_gen_avr_, ANGFSM_NAME).h)

#elif defined(__arm__)
/* Include generated header. */
#include "angfsm_generic.h"
#include XSTR (ANGFSM_PASTE_EXPAND (angfsm_gen_arm_, ANGFSM_NAME).h)

#else
/* Compiling for HOST but for direct execution. */
#include "angfsm_host_exec.h"

#endif

#include "angfsm_renaming.h"

#endif /* #ifdef _ANGFSM_GEN_ */
