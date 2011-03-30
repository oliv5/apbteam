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
import bisect

class Node:

    class closed:
        """Raised on closed connection."""
        pass

    def __init__ (self, addr = mex.DEFAULT_ADDR):
        """Create a new Node and connect it to given Hub address."""
        self.date = 0
        self.__socket = socket.socket ()
        self.__socket.setsockopt (socket.SOL_TCP, socket.TCP_NODELAY, 1)
        self.__socket.connect (addr)
        self.__seq = 0
        self.__req = None
        self.__handlers = { }
        self.__schedule_queue = [ ]
        self.register (mex.DATE, lambda msg: self.__handle_DATE (msg))
        self.register (mex.REQ, lambda msg: self.__handle_REQ (msg))
        # Synchronise.
        rsp = None
        while rsp == None or rsp.mtype != mex.DATE:
            rsp = self.__recv ()
            self.__dispatch (rsp)

    def wait (self, date = None):
        """Wait forever or until a date is reached."""
        self.__date_waited = date
        while not self.sync ():
            self.read ()

    def wait_async (self, date = None):
        """Asynchronous version of wait.  This should not be called again
        until sync return True."""
        self.__date_waited = date
        synced = self.sync ()
        assert not synced

    def sync (self):
        """To be called after read or wait_async.  Return True if the waited
        date is reached or signal the Hub our waiting status."""
        if self.date == self.__date_waited:
            return True
        else:
            idle = Msg (mex.IDLE)
            date_waited = self.__date_waited
            if self.__schedule_queue and (date_waited is None
                    or self.__schedule_queue[0][0] < date_waited):
                date_waited = self.__schedule_queue[0][0]
            if date_waited != None:
                idle.push ('L', date_waited)
            self.send (idle)

    def read (self):
        """Used for asynchronous operations.  Handle incoming data.  The sync
        method should be called after this one returns."""
        msg = self.__recv ()
        self.__dispatch (msg)

    def send (self, msg):
        """Send a message."""
        data = msg.data ()
        packet = pack (mex.HEADER_FMT, len (data), self.__seq) + data
        self.__socket.sendall (packet)

    def request (self, msg):
        """Send a request and return response."""
        # Send request.
        req = Msg (mex.REQ)
        req.push ('B', 0)
        req.push (msg.data ())
        self.send (req)
        # Wait for response.
        rsp = self.__recv ()
        while rsp.mtype != mex.RSP:
            self.__dispatch (rsp)
            rsp = self.__recv ()
        # Discard reqid.
        rsp.pop ('B')
        return Msg (rsp.pop ())

    def response (self, msg):
        """Send a response to the currently serviced request."""
        assert self.__req != None
        rsp = Msg (mex.RSP)
        rsp.push ('B', self.__req)
        self.__req = None
        rsp.push (msg.data ())
        self.send (rsp)

    def register (self, mtype, handler):
        """Register an handler for the given message type."""
        if isinstance (mtype, basestring):
            mtype = self.reserve (mtype)
        assert mtype not in self.__handlers
        self.__handlers[mtype] = handler

    def reserve (self, mtype_str):
        """Request a message type reservation."""
        # Send request.
        res = Msg (mex.RES)
        res.push (mtype_str)
        self.send (res)
        # Wait for response.
        rsp = self.__recv ()
        while rsp.mtype != mex.RES:
            self.__dispatch (rsp)
            rsp = self.__recv ()
        # Return allocated message type.
        mtype, = rsp.pop ('B')
        return mtype

    def schedule (self, date, action):
        """Schedule an action for the given date, return the event identifier."""
        assert date > self.date
        assert callable (action)
        event = date, action
        bisect.insort (self.__schedule_queue, event)
        return event

    def cancel (self, event):
        """Cancel a scheduled event."""
        self.__schedule_queue.remove (event)

    def close (self):
        """Close connection with the Hub."""
        self.__socket.close ()
        self.__socket = None

    def fileno (self):
        """Return socket fileno () for asynchronous operations."""
        return self.__socket.fileno ()

    def __recv (self):
        """Receive one message."""
        head = self.__socket.recv (calcsize (mex.HEADER_FMT))
        if head == '':
            self.close ()
            raise Node.closed
        size, self.__seq = unpack (mex.HEADER_FMT, head)
        data = self.__socket.recv (size)
        return Msg (data)

    def __dispatch (self, msg):
        """Call the right handler for the given message."""
        if msg.mtype in self.__handlers:
            self.__handlers[msg.mtype] (msg)

    def __handle_DATE (self, msg):
        """Handle an incoming DATE."""
        self.date, = msg.pop ('L')
        # Run scheduled events.
        while self.__schedule_queue \
                and self.__schedule_queue[0][0] == self.date:
            action = self.__schedule_queue[0][1]
            del self.__schedule_queue[0]
            action ()

    def __handle_REQ (self, msg):
        """Handle an incoming REQ."""
        self.__req, = msg.pop ('B')
        dec = Msg (msg.pop ())
        self.__dispatch (dec)
        self.__req = None

