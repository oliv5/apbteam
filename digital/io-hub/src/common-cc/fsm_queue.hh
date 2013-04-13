#ifndef fsm_queue_hh
#define fsm_queue_hh
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
#include "ucoolib/common.hh"
#include "fsm.hh"

/// Handle asynchronous FSM event queue.
class FsmQueue
{
  public:
    /// Constructor.
    FsmQueue ();
    /// Post an event to be processed in the next main loop.
    void post (Event e);
    /// Poll for event (return true if there is an event pending).
    bool poll () const { return nb_ != 0; }
    /// Pop one event from the queue.
    Event pop ();
  private:
    /// Maximum number of events in the queue.
    static const int queue_size_ = 8;
    /// Events to post to the FSM in next iteration.
    Event events_[queue_size_];
    /// First event in the queue.
    int head_;
    /// Number of events in the queue.
    int nb_;
};

#endif // fsm_queue_hh
