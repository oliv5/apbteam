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
"""Popen IO, to be used with a program."""
import os, time

class PopenIO:
    r"""Implements the required interface to dialog with a program.  Will
    convert from and to \r and \n."""

    def __init__ (self, cmd):
	"""Initialise and start the given commande line."""
	fout, fin = os.popen2 (cmd, 'b', 1)
	time.sleep (0.2)
	self.fin = fin
	self.fout = fout

    def read (self, *args):
	buf = self.fin.read (*args).replace ('\n', '\r')
	return buf

    def write (self, *args):
	return self.fout.write (*[i.replace ('\r', '\n') for i in args])

    def fileno (self):
	return self.fin.fileno ()

    def close (self):
	self.fin.close ()
	self.fout.close ()
