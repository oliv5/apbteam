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
"""mex message module."""
from struct import pack, unpack, calcsize

class Msg:
    r"""Msg (int) -> new empty message with given mtype.
    Msg (buffer) -> new message read from buffer.

    Examples
    ========

    Send:
    
    >>> m = Msg (3)
    >>> print m
    <Msg 03: >
    >>> m.mtype
    3
    >>> m.push ('BHl', 1, 2, 3)
    >>> print m
    <Msg 03: 01 00 02 00 00 00 03>
    >>> len (m)
    7
    >>> m.data ()
    '\x03\x01\x00\x02\x00\x00\x00\x03'

    Receive:

    >>> d = m.data ()
    >>> m2 = Msg (d)
    >>> print m2
    <Msg 03: 01 00 02 00 00 00 03>
    >>> len (m)
    7
    >>> m2.mtype
    3
    >>> m2.pop ('BHl')
    (1, 2, 3)
    >>> len (m2)
    0

    Encapsulate:

    >>> menc = Msg (4)
    >>> menc.push ('B', 5)
    >>> menc.push (d)
    >>> print menc
    <Msg 04: 05 03 01 00 02 00 00 00 03>
    >>> len (menc)
    9

    Decapsulate:

    >>> mdec = Msg (menc.data ())
    >>> mdec.pop ('B')
    (5,)
    >>> mdecd = Msg (mdec.pop ())
    >>> print mdecd
    <Msg 03: 01 00 02 00 00 00 03>
    >>> len (mdecd)
    7
    """

    def __init__ (self, f):
	"""Initialise a new message, see class documentation for
	signatures."""
	try:
	    f[0]
	except TypeError:
	    # New empty message.
	    self.mtype = f
	    self.header = pack ('!B', self.mtype)
	    self.payload = ''
	else:
	    # Read from a buffer.
	    self.header = f[0:1]
	    self.payload = f[1:]
	    self.mtype = unpack ('!B', self.header)[0]

    def data (self):
	"""Get the message data, ready to be sent."""
	return self.header + self.payload

    def __str__ (self):
	"""Return an text representation."""
	payload = ' '.join (['%02x' % ord (i) for i in self.payload])
	return '<Msg %02x: %s>' % (self.mtype, payload)

    def push (self, fmt, *args):
	"""
	Add data to the payload.
	
	msg.push (string) -> None.  Add the given string to the payload.

	msg.push (fmt, values...) -> None.  Add the given values to the
	payload, using a struct.pack format string.
	"""
	if args:
	    self.payload += pack ('!' + fmt, *args)
	else:
	    self.payload += fmt

    def pop (self, fmt = None):
	"""
	Get data from the payload.

	msg.pop () -> payload.  Get all the remaining payload.

	msg.pop (fmt) -> (values, ...).  Get values extracted according to a
	struct.unpack format string.
	"""
	if fmt:
	    nb = calcsize (fmt)
	    ex, self.payload = self.payload[0:nb], self.payload[nb:]
	    return unpack ('!' + fmt, ex)
	else:
	    ex, self.payload = self.payload, ''
	    return ex

    def __len__ (self):
	"""Get payload remaining length."""
	return len (self.payload)

def _test ():
    import doctest
    doctest.testmod ()

if __name__ == '__main__':
    _test()
