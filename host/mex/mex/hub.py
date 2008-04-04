# mex - Messages exchange library. {{{
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
"""mex Hub."""
import mex
from msg import Msg
import msg
import socket, select
from struct import pack, unpack, calcsize

class Hub:

    def __init__ (self, addr = mex.DEFAULT_ADDR, min_clients = 0, log = None):
	"""Initialise a new Hub and bind to server address."""
	self.addr = addr
	self.min_clients = min_clients
	self.log = log
	self.clients = { }
	self.next_client_id = 1
	self.date = 0
	self.socket = socket.socket ()
	self.socket.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	self.socket.setsockopt (socket.SOL_TCP, socket.TCP_NODELAY, 1)
	self.socket.bind (self.addr)
	self.socket.listen (5)

    def wait (self, cond = False):
	"""Wait until a cond is True and handle message exchanges."""
	while not cond and self.date != None:
	    self.select ()

    def select (self):
	"""Wait until the next event (connection or message) occurs and handle
	it."""
	if not self.min_clients:
	    # Check idle and wanted dates.
	    idle = True
	    min_wait_date = None
	    for c in self.clients.itervalues ():
		if not c.idle:
		    idle = False
		    break
		if c.wait_date != None and (c.wait_date < min_wait_date or
			min_wait_date == None):
		    min_wait_date = c.wait_date
	    # Abort if everyone waits forever.
	    if idle and min_wait_date == None:
		self.date = None
		return
	    # Send new date.
	    if idle:
		self.date = min_wait_date
		if self.log:
		    self.log ('[%d] date' % self.date)
		date = Msg (mex.DATE)
		date.push ('L', self.date)
		for c in self.clients.itervalues ():
		    c.send (date)
	# Prepare fdset and select.
	infds = [ self.socket ]
	if not self.min_clients:
	    infds += self.clients.values ()
	readyfds = select.select (infds, (), ()) [0]
	for i in readyfds:
	    if i is self.socket:
		self.accept ()
	    else:
		i.read ()

    def accept (self):
	"""Accept a new connection and create a client."""
	# Add client.
	a = self.socket.accept ()
	if self.log:
	    self.log ('[%d] connect from %s' % (self.date, str (a[1])))
	c = Hub.Client (self, a[0], self.next_client_id)
	self.next_client_id += 1
	assert self.next_client_id < 256
	self.clients[c.id] = c
	if self.min_clients:
	    self.min_clients -= 1
	# Send first date.
	date = Msg (mex.DATE)
	date.push ('L', self.date)
	c.send (date)

    def broadcast (self, msg, exclude = None):
	"""Send a message broadcasted, could exclude a client."""
	for c in self.clients.itervalues ():
	    if c is not exclude:
		c.send (msg)

    class Client:

	def __init__ (self, hub, socket, id):
	    """Initialise a new client."""
	    self.hub = hub
	    self.socket = socket
	    self.id = id
	    self.seq = 0
	    self.idle = True
	    self.wait_date = None

	def read (self):
	    """Read and dispatch a message from this client."""
	    # Read message.
	    head = self.socket.recv (calcsize (mex.HEADER_FMT))
	    if head == '':
		self.socket.close ()
		del self.hub.clients[self.id]
		return
	    size, seq = unpack (mex.HEADER_FMT, head)
	    data = self.socket.recv (size)
	    m = Msg (data)
	    if self.hub.log:
		self.hub.log ('[%d] received from %d(%d): %s' %
			(self.hub.date, self.id, seq, str (m)))
	    # Dispatch.
	    if m.mtype == mex.IDLE:
		if seq == self.seq:
		    self.idle = True
		    if len (m):
			self.wait_date, = m.pop ('L')
			assert self.wait_date >= self.hub.date
		    else:
			self.wait_date = None
	    elif m.mtype == mex.DATE:
		date = Msg (mex.DATE)
		date.push ('L', self.hub.date)
		self.send (date)
	    elif m.mtype == mex.REQ:
		m.pop ('B')
		mr = Msg (mex.REQ)
		mr.push ('B', self.id)
		mr.push (m.pop ())
		self.hub.broadcast (mr, self)
	    elif m.mtype == mex.RSP:
		to, = m.pop ('B')
		mr = Msg (mex.RSP)
		mr.push ('B', 0)
		mr.push (m.pop ())
		self.hub.clients[to].send (mr)
	    else:
		self.hub.broadcast (m, self)

	def send (self, msg):
	    """Send a message to this client."""
	    data = msg.data ()
	    self.seq += 1
	    if self.seq == 256:
		self.seq = 0
	    packet = pack (mex.HEADER_FMT, len (data), self.seq) + data
	    self.socket.sendall (packet)
	    self.idle = False
	    if self.hub.log:
		self.hub.log ('[%d] sending to %d(%d): %s' % (self.hub.date,
		    self.id, self.seq, str (msg)))

	def fileno (self):
	    """Return socket filedescriptor."""
	    return self.socket.fileno ()

