// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "fsm_queue.hh"

FsmQueue::FsmQueue ()
    : head_ (0), nb_ (0)
{
}

void
FsmQueue::post (Event e)
{
    ucoo::assert (nb_ < queue_size_);
    int tail = (head_ + nb_) % queue_size_;
    events_[tail] = e;
    nb_++;
}

FsmQueue::Event
FsmQueue::pop ()
{
    Event e;
    ucoo::assert (nb_ > 0);
    e = events_[head_];
    nb_--;
    head_ = (head_ + 1) % queue_size_;
    return e;
}

