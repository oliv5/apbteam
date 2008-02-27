/* uart.avr.c */
/* avr.uart - UART AVR module. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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
#include "uart.h"

#include "uart_common.h"

#if AC_UART (PORT) != -1

#include "io.h"

/* Tested AVR check. */
#if defined (__AVR_ATmega8__)
#elif defined (__AVR_ATmega8535__)
#elif defined (__AVR_ATmega16__)
#elif defined (__AVR_ATmega32__)
#elif defined (__AVR_ATmega128__)
# define PE UPE
#elif defined (__AVR_ATmega64__)
# define PE UPE
#else
# warning "uart: not tested on this chip"
#endif

/* UART port number. */
#if AC_UART (PORT) == 0
# if !defined (UBRRH)
#  if defined (UBRR0H)
#   define UBRRH UBRR0H
#   define UBRRL UBRR0L
#   define UCSRA UCSR0A
#   define UCSRB UCSR0B
#   define UCSRC UCSR0C
#   define UDR UDR0
#   define SIG_UART_RECV SIG_UART0_RECV
#   define SIG_UART_DATA SIG_UART0_DATA
#  else
#   error "uart: can not find uart 0"
#  endif /* defined (UBRR0H) */
# endif /* !defined (UBRRH) */
#else /* AC_UART (PORT) == 1 */
# if defined (UBRR1H)
#  define UBRRH UBRR1H
#  define UBRRL UBRR1L
#  define UCSRA UCSR1A
#  define UCSRB UCSR1B
#  define UCSRC UCSR1C
#  define UDR UDR1
#  define SIG_UART_RECV SIG_UART1_RECV
#  define SIG_UART_DATA SIG_UART1_DATA
# else
#  error "uart: can not find uart 1"
# endif /* defined (UBRR1H) */
#endif /* AC_UART (PORT) == 1 */

/* UR selector (UCSRC & UBRRH multiplexed). */
#if defined (URSEL)
# define SELECTOR _BV (URSEL)
#else
# define SELECTOR 0
#endif

/* Baud rate error check. */
#define UBRR_VAL (AC_FREQ / 16 / AC_UART (BAUDRATE) - 1)
#define BAUDRATE_VAL (AC_FREQ / 16 / (UBRR_VAL + 1))

#if BAUDRATE_VAL - AC_UART (BAUDRATE) > 2
#warning "uart: baud rate error > 2"
#elif BAUDRATE_VAL - AC_UART (BAUDRATE) > 1
#warning "uart: baud rate error > 1"
#elif BAUDRATE_VAL - AC_UART (BAUDRATE) > 0
#warning "uart: baud rate error > 0"
#endif

/* Mode. */
#define POLLING 1
#define RING 2
#define SEND_MODE AC_UART (SEND_MODE)
#define RECV_MODE AC_UART (RECV_MODE)
#if RECV_MODE == POLLING
# define RECV_IE 0
#elif RECV_MODE == RING
# define RECV_IE _BV (RXCIE)
#else
# error "uart: bad mode"
#endif

/* Parity. */
#define ODD (_BV (UPM1) | _BV (UPM0))
#define EVEN _BV (UPM1)
#define NONE 0
#define PARITY AC_UART (PARITY)

/* Stop bits. */
#if AC_UART (STOP_BITS) == 1
#define STOP_BITS 0
#elif AC_UART (STOP_BITS) == 2
#define STOP_BITS _BV (USBS)
#else
#error "uart: bad stop bits value"
#endif

/* Character size. */
#define CHAR_SIZE (_BV (UCSZ1) | _BV (UCSZ0))

/* Buffers. */
#if SEND_MODE == RING
# define SEND_BUFFER_SIZE AC_UART (SEND_BUFFER_SIZE)
# define SEND_BUFFER_MASK (SEND_BUFFER_SIZE - 1)
# if SEND_BUFFER_SIZE & SEND_BUFFER_MASK
#  error "uart: send buffer size must be a power of 2"
# endif
#endif
#if RECV_MODE == RING
# define RECV_BUFFER_SIZE AC_UART (RECV_BUFFER_SIZE)
# define RECV_BUFFER_MASK (RECV_BUFFER_SIZE - 1)
# if RECV_BUFFER_SIZE & RECV_BUFFER_MASK
#  error "uart: recv buffer size must be a power of 2"
# endif
#endif

/* Send buffer. */
#if SEND_MODE == RING
static uint8_t uart_send_buffer[SEND_BUFFER_SIZE];
static volatile uint8_t uart_send_head, uart_send_tail;
#endif

/* Recv buffer. */
#if RECV_MODE == RING
static uint8_t uart_recv_buffer[RECV_BUFFER_SIZE];
static volatile uint8_t uart_recv_head, uart_recv_tail;
#endif

/* Full buffer politic. */
#define DROP 1
#define WAIT 2
#define SEND_BUFFER_FULL AC_UART (SEND_BUFFER_FULL)
#if SEND_BUFFER_FULL != DROP && SEND_BUFFER_FULL != WAIT
# error "uart: bad full buffer politic"
#endif

/* Error code. */
#define ERROR_BITS (_BV (FE) | _BV (DOR) | _BV (PE))

/* +AutoDec */
/* -AutoDec */

/** Initialise uart. */
void
uart_init (void)
{
    /* Set baud rate. */
#if (UBRR_VAL >> 8) != 0
    UBRRH = UBRR_VAL >> 8;
#endif
    UBRRL = UBRR_VAL & 0xff;
    UCSRA = 0;
    /* Set format and enable uart. */
    UCSRC = SELECTOR | PARITY | STOP_BITS | CHAR_SIZE;
    UCSRB = RECV_IE | _BV (RXEN) | _BV (TXEN);
#if RECV_MODE == RING
    uart_recv_head = 0;
    uart_recv_tail = 0;
#endif
#if SEND_MODE == RING
    uart_send_head = 0;
    uart_send_tail = 0;
#endif
}

/** Read a char. */
uint8_t
uart_getc (void)
{
#if RECV_MODE == POLLING
    uint8_t tmperr, c;
    loop_until_bit_is_set (UCSRA, RXC);
    tmperr = UCSRA & ERROR_BITS;
    c = UDR;
    if (tmperr)
	return 0xff;
    else
	return c;
#elif RECV_MODE == RING
    uint8_t tmptail = uart_recv_tail;
    while (uart_recv_head == tmptail)
	;
    tmptail = (tmptail + 1) & RECV_BUFFER_MASK;
    uart_recv_tail = tmptail;
    return uart_recv_buffer[tmptail];
#endif
}

/** Write a char. */
void
uart_putc (uint8_t c)
{
#if SEND_MODE == POLLING
# if SEND_BUFFER_FULL == WAIT
    loop_until_bit_is_set (UCSRA, UDRE);
# else
    if (!(UCSRA & _BV (UDRE)))
	return;
# endif
    UDR = c;
#elif SEND_MODE == RING
    uint8_t tmphead;
    tmphead = (uart_send_head + 1) & SEND_BUFFER_MASK;
# if SEND_BUFFER_FULL == WAIT
    while (tmphead == uart_send_tail)
	;
# else
    if (tmphead == uart_send_tail)
	return;
# endif
    uart_send_buffer[tmphead] = c;
    uart_send_head = tmphead;
    UCSRB |= _BV (UDRIE);
#endif
}

/** Retrieve availlable chars. */
uint8_t
uart_poll (void)
{
#if RECV_MODE == POLLING
    return UCSRA & _BV (RXC);
#elif RECV_MODE == RING
    return (uart_recv_head - uart_recv_tail) & RECV_BUFFER_MASK;
#endif
}

#if RECV_MODE == RING

/* Handle received char for ring buffer. */
SIGNAL (SIG_UART_RECV)
{
    uint8_t c;
    uint8_t tmphead;
    uint8_t tmperr;
    tmperr = UCSRA & ERROR_BITS;
    c = UDR;
    if (tmperr)
	c = 0xff;
    tmphead = (uart_recv_head + 1) & RECV_BUFFER_MASK;
    uart_recv_head = tmphead;
    /* If overflowed, clear the receive buffer. */
    if (tmphead == uart_recv_tail)
      {
	tmphead = (tmphead + 1) & RECV_BUFFER_MASK;
	uart_recv_head = tmphead;
	c = 0xff;
      }
    uart_recv_buffer[tmphead] = c;
}

#endif /* RECV_MODE == RING */

#if SEND_MODE == RING

/** Handle data register empty for ring buffer. */
SIGNAL (SIG_UART_DATA)
{
    uint8_t tmptail;
    if (uart_send_head != uart_send_tail)
      {
	tmptail = (uart_send_tail + 1) & SEND_BUFFER_MASK;
	uart_send_tail = tmptail;
	UDR = uart_send_buffer[tmptail];
      }
    else
      {
	UCSRB &= ~_BV (UDRIE);
      }
}

#endif /* SEND_MODE == RING */

#endif /* AC_UART (PORT) != -1 */
