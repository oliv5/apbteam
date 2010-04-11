#ifndef trace_h
#define trace_h
/* trace.h */
/*  {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
/**
  * You can find the example on the website :
  * http://gcc.gnu.org/ml/gcc-patches/2000-11/msg00016.html
  */
#include "modules/trace/events.h"
#define PASTE_EXPAND(a, b) PASTE(a, b)

#define PASTE(a, b) a ## b

#define TRACE_ARGS_COUNT(...) \
    _TRACE_ARGS_COUNT1 ( , ##__VA_ARGS__)

#define _TRACE_ARGS_COUNT1(...) \
    _TRACE_ARGS_COUNT2 (__VA_ARGS__, 10,9,8,7,6,5,4,3,2,1,0)

#define _TRACE_ARGS_COUNT2(_ ,_0,_1,_2,_3,_4,_5,_6,_7,_8,_9, n,...) n

#ifndef HOST
#define TRACE(args...) \
    PASTE_EXPAND(TRACE_PRINT, TRACE_ARGS_COUNT(args...)) (args)
#else /* HOST */
#include <stdio.h>
extern char *trace_table[];
#define TRACE(id, args...) \
    do {\
        fprintf (stderr, trace_table[id], ## args);\
        PASTE_EXPAND(TRACE_PRINT, TRACE_ARGS_COUNT(id, ## args...))\
            (id, ## args);\
    } while (0)
#endif /* HOST */

#define TRACE_PRINT1(args)\
    ({TRACE_PRINT_ARG_TYPE(args);})

#define TRACE_PRINT2(arg0, arg1)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);})

#define TRACE_PRINT3(arg0, arg1, arg2)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);})

#define TRACE_PRINT4(arg0, arg1, arg2, arg3)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);\
     TRACE_PRINT_ARG_TYPE(arg3);})

#define TRACE_PRINT5(arg0, arg1, arg2, arg3, arg4)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);\
     TRACE_PRINT_ARG_TYPE(arg3);\
     TRACE_PRINT_ARG_TYPE(arg4);})

#define TRACE_PRINT6(arg0, arg1, arg2, arg3, arg4, arg5)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);\
     TRACE_PRINT_ARG_TYPE(arg3);\
     TRACE_PRINT_ARG_TYPE(arg4);\
     TRACE_PRINT_ARG_TYPE(arg5);})

#define TRACE_PRINT7(arg0, arg1, arg2, arg3, arg4, arg5, arg6)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);\
     TRACE_PRINT_ARG_TYPE(arg3);\
     TRACE_PRINT_ARG_TYPE(arg4);\
     TRACE_PRINT_ARG_TYPE(arg5);\
     TRACE_PRINT_ARG_TYPE(arg6);})

#define TRACE_PRINT_ARG_TYPE(arg)\
    do\
    {\
      if (sizeof(arg) == sizeof(uint8_t)) trace_print_arg_1(arg);\
      else if (sizeof(arg) == sizeof(uint16_t)) trace_print_arg_2(arg);\
      else if (sizeof(arg) == sizeof(uint32_t)) trace_print_arg_4(arg);\
    }while (0)

enum trace_status_t
{
    TRACE_STATUS_OFF,
    TRACE_STATUS_ON
};
typedef enum trace_status_t trace_status_t;

/** Print an argument of one byte.
  * \param  arg  the one byte argument to print.
  */
void
trace_print_arg_1(uint8_t arg);

/** Print an argument of two bytes.
  * \param  arg  the two bytes argument.
  */
void
trace_print_arg_2(uint16_t arg);

/** Print an argument of four bytes.
  * \param  arg  the four bytes argument.
  */
void
trace_print_arg_4(uint32_t arg);

/** Initialise the trace module.
  * \return   the status of the trace module.
  * Find the first sector writable and store the following start code
  * 0xF33FF22F this indicate the beginning of traces.
  */
uint8_t
trace_init (void);

/** Print the trace.
  * \param  arg  the argument to print.
  */
void
trace_print (uint8_t arg);

/** Get the current status of the trace module.
  * \return  0x1 if the module is activate, 0x0 if the module is not active.
  */
trace_status_t
trace_status (void);

/** Get the current address.
  * \return  addr  the current address managed by the trace module.
  */
uint32_t
trace_addr_current (void);

#endif /* trace_h */
