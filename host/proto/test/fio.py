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
# }}} */

class IO:
    def __init__ (self, fin = None, fout = None):
        if fin is None:
            import sys, tty
            self.fin = sys.stdin
            self.fout = sys.stdout
            tty.setcbreak (sys.stdin.fileno ())
        else:
            self.fin = fin
            self.fout = fout

    def read (self, *args):
        buf = self.fin.read (*args).replace ('\n', '\r')
        return buf

    def write (self, *args):
        return self.fout.write (*[i.replace ('\r', '\n') for i in args])

    def fileno (self):
        return self.fin.fileno ()

