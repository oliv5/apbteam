# proto - Proto interface. {{{
#
# Copyright (C) 2008 Nicolas Schodet
#
# APBTeam:
#        Web: http://apbteam.org/
#      Email: team AT apbteam DOT org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# }}}
"""Proto interface module."""
import binascii, struct, select

START = 0
BANG = 1
CMD = 2
ARG = 3

class Proto:

    def __init__ (self, file, date, timeout, log = None):
	"""Initialise and set file (serial port, pty, socket...), date
	function and timeout value.

	- file: open file connected to the slave device.
	- date: when called, should return the current time.
	- timeout: time after which retransmission is done.
	- log: if defined, will be called with a log string.

	"""
	self.file = file
	self.date = date
	self.last_send = None
	self.timeout = timeout
	self.send_queue = [ ]
	self.state = START
	self.log = log
	self.handlers = { }

    def send (self, *frame):
	"""Queue a frame to send."""
	if not self.send_queue:
	    self.last_send = None
	self.send_queue.append (Frame (*frame))

    def read (self):
	"""Read from file and receive frames."""
	for f in self.recv ():
	    if self.log:
		self.log ('recv %s' % f)
	    if self.send_queue and f == self.send_queue[0]:
		del self.send_queue[0]
		if self.send_queue:
		    self.send_head ()
	    else:
		self.dispatch (f)

    def sync (self):
	"""Send frames, return True if all is sent."""
	if self.send_queue and (self.last_send is None
		or self.last_send + self.timeout < self.date ()):
	    self.send_head ()
	return not self.send_queue

    def wait (self, cond = None):
	"""Wait forever or until cond () is True."""
	while not (self.sync () and (cond is not None or cond ())):
	    fds = select.select ((self,), (), (), self.timeout)[0]
	    for i in fds:
		assert i is self
		i.read ()

    def register (self, command, fmt, handler):
	"""Register a handler for the specified command and format.  The
	handler will receive decoded arguments."""
	key = (command, struct.calcsize ('!' + fmt))
	assert key not in self.handlers
	self.handlers[key] = (handler, fmt)

    def fileno (self):
	"""Return file descriptor, for use with select."""
	return self.file.fileno ()

    def send_head (self):
	"""Send first frame from the send queue."""
	if self.log:
	    self.log ('send %s' % self.send_queue[0])
	self.file.write (self.send_queue[0].data ())
	self.last_send = self.date ()

    def recv (self):
	"""Receive a frame, used as a generator."""
	for c in self.file.read (1):
	    if c == '!':
		self.state = BANG
	    else:
		if self.state == START:
		    pass
		elif self.state == BANG:
		    if c.isalpha ():
			self.recv_command = c
			self.recv_args = ''
			self.state = CMD
		    else:
			self.recv_error ()
		elif self.state == CMD:
		    if c == '\r':
			f = Frame (self.recv_command)
			f.args = binascii.unhexlify (self.recv_args)
			yield f
		    elif (c >= '0' and c <= '9') or (c >= 'a' and c <= 'f'):
			self.recv_args += c
			self.state = ARG
		    else:
			self.recv_error ()
		else:
		    assert self.state == ARG
		    if (c >= '0' and c <= '9') or (c >= 'a' and c <= 'f'):
			self.recv_args += c
			self.state = CMD
		    else:
			self.recv_error ()

    def recv_error (self):
	"""Handle reception errors."""
	self.state = START
	if self.log:
	    self.log ('error')
	# Resend now.
	if self.send_queue:
	    self.send_head ()

    def dispatch (self, frame):
	"""Pass a received frame to the correct handler."""
	key = (frame.command, len (frame.args))
	if key in self.handlers:
	    h = self.handlers[key]
	    h[0] (*(frame.decode (h[1])))

class Frame:

    def __init__ (self, command = None, fmt = '', *args):
	"""Initiliase a frame.  If command is given, the frame is constructed
	using a struct.pack like fmt string."""
	if command:
	    assert len (command) == 1 and command.isalpha ()
	    self.command = command
	    self.args = struct.pack ('!' + fmt, *args)
	else:
	    self.command = None
	    self.args = ''
    
    def data (self):
	"""Get a frame representation ready to send."""
	return '!' + self.command + binascii.hexlify (self.args) + '\r'

    def decode (self, fmt):
	"""Decode using a struct.unpack like fmt string."""
	return struct.unpack ('!' + fmt, self.args)

    def __eq__ (self, other):
	"""Compare for equality."""
	return self.command == other.command and self.args == other.args

    def __str__ (self):
	"""Convert to string."""
	return '!' + self.command + binascii.hexlify (self.args)
