/* output_pwm_ocr.c */
/* motor - Motor control module. {{{
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
#include "output_pwm_ocr.h"

#include "preproc.h"
#include "io.h"

/** Mask of used timer. */
#define TIMERS_MASK \
    (0 PREPROC_FOR (TIMERS_MASK_, AC_OUTPUT_PWM_OCR_LIST))
#define TIMERS_MASK_(output) TIMERS_MASK__ output
#define TIMERS_MASK__(timer, args...) | _BV (timer)

/** Test for compatible AVR. */
#if defined (__AVR_ATmega32__)
# define SUPPORTED_TIMERS 1
# define SUPPORTED_TIMERS_MASK 0b0010
#elif defined (__AVR_ATmega64__) \
    || defined (__AVR_ATmega128__) \
    || defined (__AVR_AT90USB646__) \
    || defined (__AVR_AT90USB647__) \
    || defined (__AVR_AT90USB1286__) \
    || defined (__AVR_AT90USB1287__)
# define SUPPORTED_TIMERS 1, 3
# define SUPPORTED_TIMERS_MASK 0b1010
#else
# error "motor/output/pwm_ocr: not tested on this chip"
#endif
#if (TIMERS_MASK & ~(SUPPORTED_TIMERS_MASK))
# error "motor/output/pwm_ocr: unsupported configuration"
#endif

/** Define timer test macros. */
#if TIMERS_MASK & _BV (1)
# define IF_TIMER_1(x) x
#else
# define IF_TIMER_1(x)
#endif
#if TIMERS_MASK & _BV (3)
# define IF_TIMER_3(x) x
#else
# define IF_TIMER_3(x)
#endif
#define IF_TIMER(timer, x) PREPROC_PASTE (IF_TIMER_, timer) (x)

/** Output information. */
struct output_pwm_ocr_t
{
    /** Associated output state. */
    struct output_t *output;
};
typedef struct output_pwm_ocr_t output_pwm_ocr_t;

/** Global output information. */
output_pwm_ocr_t output_pwm_ocr[PREPROC_NARG (AC_OUTPUT_PWM_OCR_LIST)];

/** Initialize hardware, to be done once. */
static void
output_pwm_ocr_init_hardware (void)
{
    static uint8_t inited;
    if (!inited)
      {
	/* Declare a variable for each used timer to receive compare output
	 * mode bits. */
#define DECLARE_TIMER(timer) \
	IF_TIMER (timer, uint8_t PREPROC_PASTE (timer_com_, timer) = 0;)
	PREPROC_FOR (DECLARE_TIMER, SUPPORTED_TIMERS);
#undef DECLARE_TIMER
	/* Configure each output, set compare output mode variables. */
#define CONFIGURE_OUTPUT(output) CONFIGURE_OUTPUT_ output
#define CONFIGURE_OUTPUT_(args...) PREPROC_NARG_CALL (CONFIGURE_OUTPUT_, args)
#define CONFIGURE_OUTPUT_7(timer, ocr, mode, pwm_io_port, pwm_io_bit, \
			   dir_io_port, dir_io_bit) \
	PREPROC_PASTE (timer_com_, timer) |= (mode) \
	    << PREPROC_PASTE (COM, timer, ocr, 0); \
	IO_OUTPUT_ (pwm_io_port, pwm_io_bit); \
	IO_OUTPUT_ (dir_io_port, dir_io_bit);
#define CONFIGURE_OUTPUT_9(timer, ocr, mode, pwm_io_port, pwm_io_bit, \
			   dir_io_port, dir_io_bit, \
			   brake_io_port, brake_io_bit) \
	CONFIGURE_OUTPUT_7 (timer, ocr, mode, pwm_io_port, pwm_io_bit, \
			    dir_io_port, dir_io_bit) \
	IO_OUTPUT_ (brake_io_port, brake_io_bit);
	PREPROC_FOR (CONFIGURE_OUTPUT, AC_OUTPUT_PWM_OCR_LIST);
#undef CONFIGURE_OUTPUT
#undef CONFIGURE_OUTPUT_
#undef CONFIGURE_OUTPUT_7
#undef CONFIGURE_OUTPUT_9
	/* Initialise used timers. */
#define WGM_BIT(timer, bit) \
	((PREPROC_PASTE (AC_OUTPUT_PWM_OCR_WGM_, timer) & _BV (bit)) \
	 ? _BV (PREPROC_PASTE (WGM, timer, bit)) : 0)
#define INIT_TIMER(timer) IF_TIMER (timer, INIT_TIMER_ (timer))
#define INIT_TIMER_(timer) \
	PREPROC_PASTE (TCCR, timer, A) = \
	    WGM_BIT (timer, 0) | WGM_BIT (timer, 1) \
	    | PREPROC_PASTE (timer_com_, timer); \
	PREPROC_PASTE (TCCR, timer, B) = \
	    WGM_BIT (timer, 2) | WGM_BIT (timer, 3) \
	    | PREPROC_PASTE (AC_OUTPUT_PWM_OCR_CS_, timer); \
	PREPROC_FOR (INIT_TIMER, SUPPORTED_TIMERS);
#undef WGM_BIT
#undef INIT_TIMER
#undef INIT_TIMER_
	/* Done. */
	inited = 1;
      }
}

void
output_pwm_ocr_init (uint8_t index, output_t *output)
{
    /* Need initialized hardware. */
    output_pwm_ocr_init_hardware ();
    /* Keep output structure for future usage. */
    output_pwm_ocr[index].output = output;
    /* Reduce maximum output if needed. */
    if (output->max > OUTPUT_MAX - AC_OUTPUT_PWM_OCR_OFFSET)
	output->max = OUTPUT_MAX - AC_OUTPUT_PWM_OCR_OFFSET;
}

/** Update a single output. */
static inline __attribute__ ((__always_inline__)) void
output_pwm_ocr_update_output (uint8_t index, volatile uint16_t *ocr,
			      volatile uint8_t *dir_io_port,
			      uint8_t dir_io_bit,
			      volatile uint8_t *brake_io_port,
			      uint8_t brake_io_bit)
{
    /* Here, there could be a problem because OCRx are double buffered, not
     * PORTx!
     * Another problem arise if the OCR sampling is done between left and
     * right OCR: the right PWM is one cycle late.
     * A solution could be to use interrupts to update PWM or to synchronise
     * general timer with PWM. */
    int16_t value = output_pwm_ocr[index].output->cur;
    if (value == 0)
      {
	*ocr = 0;
      }
    else
      {
	/* Brake is engaged on first non null value. */
	output_pwm_ocr[index].output->brake = 1;
	/* Convert signed value to sign and absolute value. */
	if (value < 0)
	  {
	    *dir_io_port &= ~_BV (dir_io_bit);
	    *ocr = -value + AC_OUTPUT_PWM_OCR_OFFSET;
	  }
	else
	  {
	    *dir_io_port |= _BV (dir_io_bit);
	    *ocr = value + AC_OUTPUT_PWM_OCR_OFFSET;
	  }
      }
    /* Update brake. */
    if (brake_io_port)
      {
	if (output_pwm_ocr[index].output->brake)
	    *brake_io_port |= _BV (brake_io_bit);
	else
	    *brake_io_port &= ~_BV (brake_io_bit);
      }
}

void
output_pwm_ocr_update (void)
{
    /* Update each output, code will be optimized by compiler. */
#define UPDATE_OUTPUT(index, output) \
    output_pwm_ocr_update_output (index, UPDATE_OUTPUT_ output);
#define UPDATE_OUTPUT_(timer, ocr, mode, pwm_io_port, pwm_io_bit, \
		       dir_io_port, dir_io_bit, args...) \
    &PREPROC_PASTE (OCR, timer, ocr), &IO_PORT_ (dir_io_port, dir_io_bit), \
    dir_io_bit, PREPROC_NARG_CALL (UPDATE_OUTPUT_BRAKE_, args)
#define UPDATE_OUTPUT_BRAKE_0() 0, 0
#define UPDATE_OUTPUT_BRAKE_2(brake_io_port, brake_io_bit) \
    &IO_PORT_ (brake_io_port, brake_io_bit), brake_io_bit
    PREPROC_FOR_ENUM (UPDATE_OUTPUT, AC_OUTPUT_PWM_OCR_LIST)
#undef UPDATE_OUTPUT
#undef UPDATE_OUTPUT_
#undef UPDATE_OUTPUT_BRAKE_0
#undef UPDATE_OUTPUT_BRAKE_2
}

