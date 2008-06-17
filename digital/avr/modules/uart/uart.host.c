/* uart.host.c */
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
#define _GNU_SOURCE
#include "common.h"
#include "uart.h"
#include "modules/host/host.h"

#include "uart_common.h"

#if AC_UART (PORT) != -1

#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <pty.h>
#include <fcntl.h>
#include <stdio.h>

/* Check uart driver. */
#if UART_N == 0
# define STDIO 1
# define PTS 2
# if AC_UART (HOST_DRIVER) == STDIO
#  define DRIVER_STDIO
# elif AC_UART (HOST_DRIVER) == PTS
# else
#  error "uart: invalid host driver"
# endif
#endif

#ifndef DRIVER_STDIO

/* Pseudo terminal file descriptor. */
static int uart_pt_fd;

/** Setup a non canonical mode. */
static void
setup_raw (int fd)
{
    struct termios tc;
    tcgetattr (fd, &tc);
    tc.c_iflag &= ~(IGNPAR | PARMRK | ISTRIP | IGNBRK | BRKINT | IGNCR |
		    ICRNL | INLCR | IXON | IXOFF | IXANY | IMAXBEL);
    tc.c_iflag |= INPCK;
    tc.c_oflag &= ~(OPOST);
    tc.c_cflag &= ~(HUPCL | CSTOPB | PARENB | PARODD | CSIZE);
    tc.c_cflag |= CS8 | CLOCAL | CREAD;
    tc.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN | NOFLSH | TOSTOP);
    tc.c_cc[VTIME] = 0;
    tc.c_cc[VMIN] = 1;
    tcflush (fd, TCIFLUSH);
    tcsetattr (fd, TCSANOW, &tc);
}

#endif /* DRIVER_STDIO */

/** Initialise uart. */
void
uart_init (void)
{
#ifdef DRIVER_STDIO
    int r;
# define uart_pt_fd_in 0
# define uart_pt_fd_out 1
    /* Always use line buffering. */
    r = setvbuf (stdout, 0, _IOLBF, BUFSIZ);
    assert (r == 0);
#else
# define uart_pt_fd_in uart_pt_fd
# define uart_pt_fd_out uart_pt_fd
    int slave_fd;
    const char *name;
#define STRINGIFY(x) #x
#define FILE_NAME(x) "uart" STRINGIFY(x) ".pts"
    /* Try to access an already opened pts. */
    uart_pt_fd = host_fetch_integer (FILE_NAME (UART_N));
    if (uart_pt_fd == -1)
      {
	/* Open and unlock pt. */
	if (openpty (&uart_pt_fd, &slave_fd, 0, 0, 0) == -1
	    || grantpt (uart_pt_fd) == -1
	    || unlockpt (uart_pt_fd) == -1)
	    assert_perror (errno);
	/* Make a link to the slave pts. */
	unlink (FILE_NAME (UART_N));
	name = ptsname (uart_pt_fd);
	assert (name);
	if (symlink (name, FILE_NAME (UART_N)) == -1)
	    assert_perror (errno);
	/* Register the file descriptor in case of reset. */
	host_register_integer (FILE_NAME (UART_N), uart_pt_fd);
	/* Make slave raw. */
	setup_raw (slave_fd);
	/* slave_fd is left open. */
      }
#endif
}

/** Read a char. */
uint8_t
uart_getc (void)
{
    uint8_t c;
    int n;
    n = read (uart_pt_fd_in, &c, 1);
    if (n == -1)
	assert_perror (errno);
#ifdef DRIVER_STDIO
    /* Quit if EOF from stdin. */
    if (n == 0)
	exit (0);
    if (c == '\n')
	c = '\r';
#else
    /* This is a quite unusual and dangerous behavior... */
    if (n == 0)
	return 0xff;
#endif
    return c;
}

/** Write a char. */
void
uart_putc (uint8_t c)
{
#ifdef DRIVER_STDIO
    if (c == '\r')
	c = '\n';
#endif
    write (uart_pt_fd_out, &c, 1);
}

/** Retrieve available chars. */
uint8_t
uart_poll (void)
{
    fd_set fds;
    struct timeval tv;
    int r;
    /* Use select to poll. */
    FD_ZERO (&fds);
    FD_SET (uart_pt_fd_in, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    r = select (FD_SETSIZE, &fds, 0, 0, &tv);
    /* Check result. */
    if (r == -1)
	assert_perror (errno);
    return r;
}

#endif /* AC_UART (PORT) != -1 */
