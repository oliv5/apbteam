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
"""mex - Messages exchange library."""

IDLE = 0
"""IDLE message, send by nodes to the hub to signal it had handled all
messages.  It can have an optionnal parameter: the date at which the node will
not be idle anymore."""

DATE = 1
"""DATE message, send by the hub to the nodes to update the current date
value."""

REQ = 2
"""REQ (request) message, send from a node to other nodes to request a
response message."""

RSP = 3
"""RSP (response) message, response to a REQ message, only sent to the
requesting node."""

DEFAULT_ADDR = ('localhost', 2442)

HEADER_FMT = '!HB'
