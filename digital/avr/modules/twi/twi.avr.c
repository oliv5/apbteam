/* twi.avr.c */
/* avr.twi - TWI AVR module. {{{
 *
 * Copyright (C) 2005 Demonchy Clément
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
#include "twi.h"
#include "io.h"

#if !AC_TWI_MASTER_ENABLE && !AC_TWI_SLAVE_ENABLE
# error "twi: you should enable at least master or slave code"
#endif

#if AC_TWI_MASTER_ENABLE && AC_TWI_SLAVE_ENABLE
# error "twi: multi-master not implemented"
#endif

#include <compat/twi.h>
// #include "modules/uart/uart.h"

// TODO mettre l'etat partout 
// quand on fait les demandes, et les liberations qui vont bien dans
// l'interruption

enum
{
    TWI_SUCCESS,
    TWI_BUSY,
    TWI_FREE,
    TWI_FAILURE
};

#if AC_TWI_SLAVE_ENABLE
/* données reçues du master */
static volatile uint8_t rcpt_buf_sl[AC_TWI_SL_RECV_BUFFER_SIZE];
static volatile uint8_t data_ready_sl;
/* données à envoyer au master */
/* 2 buffers 1 pour envoyer vers l'user et l'autre vers l'i2c */
static volatile uint8_t send_buf_sl1[AC_TWI_SL_SEND_BUFFER_SIZE];
static volatile uint8_t send_buf_sl2[AC_TWI_SL_SEND_BUFFER_SIZE];
static volatile uint8_t *send_sys_sl, *send_user_sl;
static volatile uint8_t update_sl; /* lock pour savoir si on peut switcher les buffers sans risque */
static volatile uint8_t send_switch_sl;
static volatile uint8_t rcpt_idx_sl = 0;
#endif /* AC_TWI_SLAVE_ENABLE */
#if AC_TWI_MASTER_ENABLE
static volatile int8_t state_ms;
static volatile int8_t nb_retry_ms = 5; // XXX utile ?
static volatile uint8_t dest_addr_ms;
static volatile uint8_t len_buf_ms;
static volatile uint8_t *send_buf_ms;
#endif /* AC_TWI_MASTER_ENABLE */

void
twi_init (uint8_t addr)
{
#if AC_TWI_SLAVE_ENABLE
    data_ready_sl = 0;
    send_switch_sl = 0;
    update_sl = 0;
    send_sys_sl = send_buf_sl1;
    send_user_sl = send_buf_sl2;
    TWAR = addr;
    TWSR = 0x00;
    TWCR = _BV(TWEA);
#endif /* AC_TWI_SLAVE_ENABLE */
#if AC_TWI_MASTER_ENABLE
 #if defined(TWPS0)
     TWSR = 0;
 #endif
     TWBR = (AC_FREQ / 100000UL - 16) / 2;
    state_ms = TWI_FREE;
#endif /* AC_TWI_MASTER_ENABLE */
    /* Active la twi et les interruptions de la twi */
    TWCR |= _BV (TWEN) | _BV (TWIE); 
}

#if AC_TWI_SLAVE_ENABLE
uint8_t 
twi_sl_poll (uint8_t *buffer, uint8_t size)
{
    // XXX state
    if (data_ready_sl)
      {
	data_ready_sl = 0;
	uint8_t to_read, read;
	/* Get how many data we can really read. */
	if (size < rcpt_idx_sl)
	    /* FIXME: this case sucks. */
	    to_read = size;
	else
	    to_read = rcpt_idx_sl;
	/* Copy amount of data read. */
	read = to_read;
	while (to_read --)
	    buffer[to_read] = rcpt_buf_sl[to_read];
	/* de nouveau dispo pour traiter de nouvelles requetes */
	TWCR |= _BV (TWEA); 
	return read;
      }
    else
	return 0;
}
#endif  /* AC_TWI_SLAVE_ENABLE */

#if AC_TWI_SLAVE_ENABLE
void
twi_sl_update (uint8_t *buffer, uint8_t size)
{
    // XXX state
    update_sl = 1;
    send_switch_sl = 1; /* demander un swap des buffers */
    while (size --)
	send_user_sl[size] = buffer[size];
    update_sl = 0;
}
#endif  /* AC_TWI_SLAVE_ENABLE */

#if AC_TWI_MASTER_ENABLE
int8_t 
twi_ms_is_finished ()
{
    if (state_ms == TWI_SUCCESS || state_ms == TWI_FAILURE)
      {
	state_ms = TWI_FREE;
	return 1;
      }
    else
	return 0;
}
#endif /* AC_TWI_MASTER_ENABLE */

#if AC_TWI_MASTER_ENABLE
int8_t 
twi_ms_send (uint8_t addr, uint8_t *data, uint8_t len)
{
     //uart0_putc ('a');
    if (state_ms != TWI_BUSY)
      {
	//uart0_putc ('b');
	state_ms = TWI_BUSY;
	nb_retry_ms = 0;
	dest_addr_ms = addr & 0xfe;
	len_buf_ms = len;
	send_buf_ms = data;
	/* envoie du start */
	TWCR |= _BV(TWSTA) | _BV(TWINT);
	return 0;
      }
    else 
      {
	//uart0_putc ('c');
	return -1;
      }
}
#endif /* AC_TWI_MASTER_ENABLE */

#if AC_TWI_MASTER_ENABLE
int8_t
twi_ms_read (uint8_t addr, uint8_t *data, uint8_t len)
{

	//uart0_putc ('z');
    if (state_ms != TWI_BUSY)
      {
	//uart0_putc ('y');
	state_ms = TWI_BUSY;
	nb_retry_ms = 0;
	dest_addr_ms = addr | 0x01;
	len_buf_ms = len;
	send_buf_ms = data;
	TWCR |= _BV(TWSTA) | _BV (TWINT);
	return 0;
      }
    else
      {
	//uart0_putc ('x');
	return -1;
      }
}
#endif /* AC_TWI_MASTER_ENABLE */

SIGNAL (SIG_2WIRE_SERIAL)
{
#if AC_TWI_SLAVE_ENABLE
    static uint8_t send_idx_sl = 0;
#endif  /* AC_TWI_SLAVE_ENABLE */
#if AC_TWI_MASTER_ENABLE 
    static uint8_t idx_ms;
#define NB_RETRY 5
#endif /* AC_TWI_MASTER_ENABLE */
    switch (TW_STATUS)
      {
#if AC_TWI_SLAVE_ENABLE
	/***** slave transmitter mode *****/
	/* START + SLA|W + ACK 
	 * on a recu un start pour une ecriture et on a acquité 
	 * choisi le buffer d'envoie
	 * envoie le premier byte 
	 */
      case TW_ST_SLA_ACK:
      case TW_ST_ARB_LOST_SLA_ACK:
	if (send_switch_sl && !update_sl) 
	  {
	    volatile uint8_t *tmp = send_sys_sl;
	    send_sys_sl = send_user_sl;
	    send_user_sl = tmp;
	    send_switch_sl = 0;
	  }
	send_idx_sl = 0;
	/* NO BREAK */
	/* Recu un ack apres l'envoie d'un bit */
      case TW_ST_DATA_ACK:
	//uart0_putc ('p');
	TWDR = send_sys_sl[send_idx_sl++];
	if (send_idx_sl == AC_TWI_SL_SEND_BUFFER_SIZE)
	    TWCR &= ~_BV(TWEA);
	TWCR |= _BV(TWINT); 
	break;
      case TW_ST_DATA_NACK:
      case TW_ST_LAST_DATA:
	//uart0_putc ('q');
	TWCR |= _BV (TWEA);
	TWCR |= _BV(TWINT);
	break;
	/***** slave receiver mode *****/
	/* START + SLA|W + ACK */
      case TW_SR_SLA_ACK:
      case TW_SR_ARB_LOST_SLA_ACK:
      case TW_SR_GCALL_ACK:
      case TW_SR_ARB_LOST_GCALL_ACK:
	//uart0_putc ('u');
	data_ready_sl = 0;
	rcpt_idx_sl = 0;
	if (AC_TWI_SL_RECV_BUFFER_SIZE == 1)
	    TWCR &= ~_BV(TWEA);
	TWCR |= _BV(TWINT);
	break;
	/* DATA + ACK */
      case TW_SR_DATA_ACK:
      case TW_SR_GCALL_DATA_ACK:
	//uart0_putc ('s');
	rcpt_buf_sl[rcpt_idx_sl++] = TWDR;
	if (AC_TWI_SL_RECV_BUFFER_SIZE - rcpt_idx_sl == 1)
	    TWCR &= ~_BV(TWEA);
	TWCR |= _BV(TWINT);
	break;
	/* DATA + NACK */
      case TW_SR_DATA_NACK:
      case TW_SR_GCALL_DATA_NACK:
	//uart0_putc ('o');
	rcpt_buf_sl[rcpt_idx_sl++] =  TWDR;
	/* NO BREAK */
	/* STOP */
      case TW_SR_STOP:
	//uart0_putc ('q');
	data_ready_sl = 1;
	TWCR &= ~_BV(TWEA);
	TWCR |= _BV(TWINT);
	break;
#endif /* AC_TWI_SLAVE_ENABLE */
#if AC_TWI_MASTER_ENABLE
	/* Master */
      case TW_START: 
      case TW_REP_START:
	//uart0_putc ('d');
	/* start transmis, on envoie l'adresse */
	TWCR &= ~ (_BV (TWSTA) |  _BV (TWSTO) | _BV (TWINT));
	TWDR = dest_addr_ms;
	idx_ms = 0;
	TWCR |= _BV (TWINT);
	break;
      case TW_MT_ARB_LOST: /* valable pour le receiver aussi */
	/* todo */
	break;
	/* Master Transmitter */
      case TW_MT_DATA_ACK:
      case TW_MT_SLA_ACK: 
	//uart0_putc ('e');
	/* start transmis, on envoie l'adresse */
	/* slave ok 
	 * On envoi des données si il en reste
	 */
	if (idx_ms < len_buf_ms) 
	  {
	    TWDR = send_buf_ms[idx_ms ++];
	  }
	else
	  {
	    TWCR  |= _BV (TWSTO);
	    state_ms = TWI_SUCCESS;
	  }
	TWCR |= _BV (TWINT);
	break;
      case TW_MT_SLA_NACK:
	//uart0_putc ('f');
	/* start transmis, on envoie l'adresse */
	/* le slave ne repond plus 
	 * on essaye NB_RETRY avant d'arreter 
	 */
	if (nb_retry_ms  < NB_RETRY)
	  {
	    TWCR |= _BV (TWSTA);
	    nb_retry_ms ++;
	  }
	else
	  {
	    TWCR  |= _BV (TWSTO);
	    state_ms = TWI_FAILURE;
	  }
	TWCR |= _BV (TWINT);
	break;
      case TW_MT_DATA_NACK:
	//uart0_putc ('g');
	/* start transmis, on envoie l'adresse */
	/* le slave ne veut plus de donnée */
	TWCR  |= _BV (TWSTO);
	state_ms = TWI_SUCCESS;
	TWCR |= _BV (TWINT);
	break;
	/* Master Receiver */
      case TW_MR_SLA_ACK:
	//uart0_putc ('h');
	/* start transmis, on envoie l'adresse */
	if (len_buf_ms > 1) /* on recoit plusieurs octet */
	  {
	    TWCR |= _BV (TWEA);
	  }
	TWCR |= _BV (TWINT);
	break;
      case TW_MR_SLA_NACK:
	/* start transmis, on envoie l'adresse */
	if (nb_retry_ms < NB_RETRY)
	  {
	//uart0_putc ('i');
	    TWCR |= _BV (TWEA);
	  }
	else 
	  {
	//uart0_putc ('j');
	    TWCR |= _BV (TWSTO);
	    state_ms = TWI_FAILURE;
	  }
	TWCR |= _BV (TWINT);
	break;
      case TW_MR_DATA_ACK:
	//uart0_putc ('k');
	send_buf_ms[idx_ms ++] = TWDR;
	if (idx_ms == (len_buf_ms - 1)) 
	    TWCR &= ~ _BV (TWEA);
	TWCR |= _BV (TWINT);
	break;
      case TW_MR_DATA_NACK: /* dernier byte */
	//uart0_putc ('l');
	state_ms = TWI_SUCCESS;
	send_buf_ms[idx_ms ++] = TWDR;
	TWCR |= _BV (TWSTO);
	TWCR |= _BV (TWINT);
	break;
#endif/* AC_TWI_MASTER_ENABLE */
      }
}
