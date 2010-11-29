# dev2 - Multi-purpose development board using USB and Ethernet. {{{
#
# Copyright (C) 2010 Nicolas Schodet
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
"""TWI interface."""

import binascii
import struct

class ProtocolError (RuntimeError):
    pass

class Twi:
    """Interface to dev2 twi module."""

    def __init__ (self, fd):
        """Initialise and set file object to use."""
        self.fd = fd

    def _send_command (self, s):
        """Send command to dev2, return response."""
        self.fd.write (s)
        r = self.fd.readline (eol = '\r')
        if '!' in r:
            raise ProtocolError
        else:
            return r

    def send (self, address, data):
        """Send data to specified slave address."""
        s = ''.join (('s%02x' % address, binascii.hexlify (data), '\r'))
        r = self._send_command (s)
        if len (r) != 4 or r[0] != 'S' or r[-1] != '\r':
            raise ProtocolError
        try:
            return int (r[1:-1], 16)
        except ValueError:
            raise ProtocolError

    def recv (self, address, size):
        """Receive data from specified slave address."""
        s = 'r%02x%02x\r' % (address, size)
        r = self._send_command (s)
        if len (r) < 2 or r[0] != 'R' or r[-1] != '\r':
            raise ProtocolError
        try:
            return binascii.unhexlify (r[1:-1])
        except TypeError:
            raise ProtocolError

