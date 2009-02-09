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
"""mex Node."""
import mex
from msg import Msg
import socket
from struct import pack, unpack, calcsize

class Node:

    class closed:
        """Raised on closed connection."""
        pass

    def __init__ (self, addr = mex.DEFAULT_ADDR):
        """Create a new Node and connect it to given Hub address."""
        self.socket = socket.socket ()
        self.socket.setsockopt (socket.SOL_TCP, socket.TCP_NODELAY, 1)
        self.socket.connect (addr)
        self.date = 0
        self.seq = 0
        self.req = None
        self.handlers = { }
        self.register (mex.DATE, lambda msg: self.handle_DATE (msg))
        self.register (mex.REQ, lambda msg: self.handle_REQ (msg))
        # Synchronise.
        rsp = None
        while rsp == None or rsp.mtype != mex.DATE:
            rsp = self.recv ()
            self.dispatch (rsp)

    def wait (self, date = None):
        """Wait forever or until a date is reached."""
        while date == None or self.date != date:
            idle = Msg (mex.IDLE)
            if date != None:
                idle.push ('L', date)
            self.send (idle)
            msg = self.recv ()
            self.dispatch (msg)

    def wait_async (self, date = None):
        """Asynchronous version of wait.  This should not be called again
        until sync return True."""
        self.async_waited = date
        synced = self.sync ()
        assert not synced

    def sync (self):
        """To be called after read or wait_async.  Return True if the waited
        date is reached or signal the Hub our waiting status."""
        if self.date == self.async_waited:
            return True
        else:
            idle = Msg (mex.IDLE)
            if self.async_waited != None:
                idle.push ('L', self.async_waited)
            self.send (idle)

    def read (self):
        """Used for asynchronous operations.  Handle incoming data.  The sync
        method should be called after this one returns."""
        msg = self.recv ()
        self.dispatch (msg)

    def send (self, msg):
        """Send a message."""
        data = msg.data ()
        packet = pack (mex.HEADER_FMT, len (data), self.seq) + data
        self.socket.sendall (packet)

    def request (self, msg):
        """Send a request and return response."""
        # Send request.
        req = Msg (mex.REQ)
        req.push ('B', 0)
        req.push (msg.data ())
        self.send (req)
        # Wait for response.
        rsp = self.recv ()
        while rsp.mtype != mex.RSP:
            self.dispatch (rsp)
            rsp = self.recv ()
        # Discard reqid.
        rsp.pop ('B')
        return Msg (rsp.pop ())

    def response (self, msg):
        """Send a response to the currently serviced request."""
        assert self.req != None
        rsp = Msg (mex.RSP)
        rsp.push ('B', self.req)
        self.req = None
        rsp.push (msg.data ())
        self.send (rsp)

    def register (self, mtype, handler):
        """Register an handler for the given message type."""
        assert mtype not in self.handlers
        self.handlers[mtype] = handler

    def close (self):
        """Close connection with the Hub."""
        self.socket.close ()
        self.socket = None

    def fileno (self):
        """Return socket fileno () for asynchronous operations."""
        return self.socket.fileno ()

    def recv (self):
        """Receive one message."""
        head = self.socket.recv (calcsize (mex.HEADER_FMT))
        if head == '':
            self.close ()
            raise Node.closed
        size, self.seq = unpack (mex.HEADER_FMT, head)
        data = self.socket.recv (size)
        return Msg (data)

    def dispatch (self, msg):
        """Call the right handler for the given message."""
        if msg.mtype in self.handlers:
            self.handlers[msg.mtype] (msg)

    def handle_DATE (self, msg):
        """Handle an incoming DATE."""
        self.date, = msg.pop ('L')

    def handle_REQ (self, msg):
        """Handle an incoming REQ."""
        self.req, = msg.pop ('B')
        dec = Msg (msg.pop ())
        self.dispatch (dec)
        self.req = None

