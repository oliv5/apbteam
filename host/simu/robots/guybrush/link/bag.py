# simu - Robot simulation. {{{
#
# Copyright (C) 2012 Nicolas Schodet
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
"""Guybrush bag of links."""
import io_hub.mex
import asserv.mex
import mimot.mex

class Bag:

    def __init__ (self, node, instance = 'robot0'):
        self.asserv = asserv.mex.Mex (node, '%s:asserv0' % instance)
        self.io_hub = io_hub.mex.Mex (node, '%s:io0' % instance,
                contact_nb = 3)
        self.mimot = mimot.mex.Mex (node, '%s:mimot0' % instance)

