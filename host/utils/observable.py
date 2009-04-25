# host/utils - Host utilities. {{{
#
# Copyright (C) 2009 Alexandre Stanislawski
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
"""Observable/Observer pattern."""

class Observable:
    """Derive from this object to be observable.  An observable object will
    notify each of its observers every time it changes state."""

    def __init__ (self):
        self.__observers = [ ]

    def register (self, observer):
        """Register an observer to this observable."""
        assert callable (observer)
        self.__observers.append (observer)

    def notify (self):
        """Notify all observers.  This should be called after every state
        change."""
        for observer in self.__observers:
            observer ()
