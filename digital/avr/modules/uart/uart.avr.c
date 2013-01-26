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
#elif defined (__AVR_ATmega164P__)
#elif defined (__AVR_ATmega32__)
#elif defined (__AVR_ATmega64__)
#elif defined (__AVR_ATmega128__)
#elif defined (__AVR_AT90USB162__)
#elif defined (__AVR_AT90USB646__)
#elif defined (__AVR_AT90USB647__)
#elif defined (__AVR_AT90USB1286__)
#elif defined (__AVR_AT90USB1287__)
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
#   define USART_RX_vect USART0_RX_vect
#   define USART_UDRE_vect USART0_UDRE_vect
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
#  define USART_RX_vect USART1_RX_vect
#  define USART_UDRE_vect USART1_UDRE_vect
# else
#  error "uart: can not find uart 1"
# endif /* defined (UBRR1H) */
#endif /* AC_UART (PORT) == 1 */

/* Missing generic definitions. */
#if !defined (TXEN)
# if AC_UART (PORT) == 0
#  define RXC RXC0
#  define TXC TXC0
#  define UDRE UDRE0
#  define FE FE0
#  define DOR DOR0
#  define UPE UPE0
#  define U2X U2X0
#  define MPCM MPCM0
#  define RXCIE RXCIE0
#  define TXCIE TXCIE0
#  define UDRIE UDRIE0
#  define RXEN RXEN0
#  define TXEN TXEN0
#  define UCSZ2 UCSZ02
#  define RXB8 RXB80
#  define TXB8 TXB80
#  define UMSEL1 UMSEL01
#  define UMSEL0 UMSEL00
#  define UPM1 UPM01
#  define UPM0 UPM00
#  define USBS USBS0
#  define UCSZ1 UCSZ01
#  define UCSZ0 UCSZ00
#  define UCPOL UCPOL0
# else /* AC_UART (PORT) == 1 */
#  define RXC RXC1
#  define TXC TXC1
#  define UDRE UDRE1
#  define FE FE1
#  define DOR DOR1
#  define UPE UPE1
#  define U2X U2X1
#  define MPCM MPCM1
#  define RXCIE RXCIE1
#  define TXCIE TXCIE1
#  define UDRIE UDRIE1
#  define RXEN RXEN1
#  define TXEN TXEN1
#  define UCSZ2 UCSZ12
#  define RXB8 RXB81
#  define TXB8 TXB81
#  define UMSEL1 UMSEL11
#  define UMSEL0 UMSEL10
#  define UPM1 UPM11
#  define UPM0 UPM10
#  define USBS USBS1
#  define UCSZ1 UCSZ11
#  define UCSZ0 UCSZ10
#  define UCPOL UCPOL1
# endif /* AC_UART (PORT) == 1 */
#endif /* !defined (TXEN) */

/* Different names for different chips. */
#ifndef UPE
# define UPE PE
#endif
#ifndef USART_RX_vect
# define USART_RX_vect USART_RXC_vect
#endif

/* UR selector (UCSRC & UBRRH multiplexed). */
#if defined (URSEL)
# define SELECTOR _BV (URSEL)
#else
# define SELECTOR 0
#endif

/* Baud rate error check. */
#define UBRR_VAL (AC_FREQ / 16 / AC_UART (BAUDRATE) - 1)
#define BAUDRATE_VAL (AC_FREQ / 16 / (UBRR_VAL + 1))
#define BAUDRATE_ERROR (BAUDRATE_VAL - AC_UART (BAUDRATE))
#define BAUDRATE_ERROR_PCT (BAUDRATE_ERROR * 100 / AC_UART (BAUDRATE))

#if BAUDRATE_ERROR_PCT >= 2
#warning "uart: baud rate error > 2 %"
#elif BAUDRATE_ERROR_PCT >= 1
#warning "uart: baud rate error > 1 %"
#elif BAUDRATE_ERROR > 2
#warning "uart: baud rate error > 2"
#elif BAUDRATE_ERROR > 1
#warning "uart: baud rate error > 1"
#elif BAUDRATE_ERROR > 0
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
#define STOP 0
#elif AC_UART (STOP_BITS) == 2
#define STOP _BV (USBS)
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
#define ERROR_BITS (_BV (FE) | _BV (DOR) | _BV (UPE))

/* +AutoDec */
/* -AutoDec */

/** Initialise uart. */
void
uart_init (void)
{
    /* Set baud rate. */
    UBRRH = UBRR_VAL >> 8;
    UBRRL = UBRR_VAL & 0xff;
    UCSRA = 0;
    /* Set format and enable uart. */
    UCSRC = SELECTOR | PARITY | STOP | CHAR_SIZE;
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

/** Change uart speed after init. */
void
uart_set_speed (uint32_t speed)
{
    uint16_t ubrr_val = AC_FREQ / 16 / speed - 1;
    UBRRH = ubrr_val >> 8;
    UBRRL = ubrr_val & 0xff;
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

/** Retrieve available chars. */
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
SIGNAL (USART_RX_vect)
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
SIGNAL (USART_UDRE_vect)
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
