#!/usr/bin/python
# dev2 - Multi-purpose development board using USB and Ethernet. {{{
#
# Copyright (C) 2009 Nicolas Schodet
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
"""Control dev2 board using USB."""
import sys
from optparse import OptionParser
import usb

# Parse options.
opt = OptionParser (description = __doc__)
opt.add_option ('-s', '--select', type = 'int',
        help = 'select specified OUTPUT (1 to 4)', metavar = 'OUTPUT')
opt.add_option ('-u', '--unselect', action = 'store_true', default = False,
        help = 'unselect outputs')
opt.add_option ('-d', '--dfu', action = 'store_true', default = False,
        help = 'go to DFU boot loader')

(options, args) = opt.parse_args ()
if args:
    opt.error ('too many arguments')
if (options.select is not None) + options.unselect + options.dfu != 1:
    opt.error ('choose one of available options')
if options.select is not None and (options.select < 1 or options.select > 4):
    opt.error ('output out of bound')
if options.unselect:
    options.select = 0

# Open device.
d = None
for bus in usb.busses ():
    for dev in bus.devices:
        if dev.idVendor == 0x03eb and dev.idProduct == 0x204e:
            d = dev.open ()
if d is None:
    print >> sys.stderr, 'device not found'
    sys.exit (1)

# Send control message.
if options.dfu:
    d.controlMsg (usb.TYPE_VENDOR | usb.RECIP_DEVICE, 0, 0)
elif options.select is not None:
    d.controlMsg (usb.TYPE_VENDOR | usb.RECIP_DEVICE, 1, 0,
            value = options.select)
else:
    assert 0
