import proto, time
import numpy
import math

class Asserv:

    stats_format = {
            'C': 'HHH',
            'Z': 'H',
            'S': 'bbb',
            'P': 'hhhhhh',
            'W': 'hhh',
            }
    # The last occuring stats will increment stats_count, so they have to
    # be in the same order than in asserv program.
    stats_order = 'CZSPW'
    stats_items = {
            'lc': ('C', 0),
            'rc': ('C', 1),
            'a0c': ('C', 2),
            'a0z': ('Z', 0),
            'ts': ('S', 0),
            'as': ('S', 1),
            'a0s': ('S', 2),
            'te': ('P', 0),
            'ti': ('P', 1),
            'ae': ('P', 2),
            'ai': ('P', 3),
            'a0e': ('P', 4),
            'a0i': ('P', 5),
            'lw': ('W', 0),
            'rw': ('W', 1),
            'a0w': ('W', 2),
            }

    def __init__ (self, file, time = time.time, **param):
        self.proto = proto.Proto (file, time, 0.1)
        self.async = False
        self.mseq = 0
        self.mseq_ack = 0
        self.a0seq = 0
        self.a0seq_ack = 0
        self.proto.register ('A', 'BB', self.handle_ack)
        def make_handle (s):
            return lambda *args: self.handle_stats (s, *args)
        for (s, f) in self.stats_format.iteritems ():
            self.proto.register (s, f, make_handle (s))
        self.stats_enabled = None
        self.param = dict (
                scale = 1,
                tkp = 0, tki = 0, tkd = 0,
                akp = 0, aki = 0, akd = 0,
                a0kp = 0, a0ki = 0, a0kd = 0,
                E = 1023, I = 1023, D = 1023,
                be = 2048, bs = 0x10, bc = 20,
                ta = 1, aa = 1, a0a = 1,
                tsm = 0, asm = 0, tss = 0, ass = 0, a0sm = 0, a0ss = 0,
                c = 1, f = 0x1000,
                l = 0x2000,
                )
        self.param.update (param)
        self.send_param ()

    def stats (self, *stats_items, **options):
        """Activate stats."""
        interval = 1
        if 'interval' in options:
            interval = options['interval']
        # Build list of stats letters.
        stats = [self.stats_items[i][0] for i in stats_items]
        stats = [s for s in self.stats_order if s in stats]
        stats_last_pos = 0
        stats_pos = { }
        for s in stats:
            stats_pos[s] = stats_last_pos
            stats_last_pos += len (self.stats_format[s])
        # Build stats item positions.
        self.stats_items_pos = [ ]
        for i in stats_items:
            id = self.stats_items[i]
            self.stats_items_pos.append (stats_pos[id[0]] + id[1])
        # Enable stats.
        for s in stats:
            self.proto.send (s, 'B', interval)
        # Prepare aquisition.
        self.stats_enabled = stats
        self.stats_counter = stats[-1]
        self.stats_count = 0
        self.stats_list = [ ]
        self.stats_line = [ ]

    def get_stats (self, wait = None):
        if wait:
            self.wait (wait)
        list = self.stats_list
        # Drop first line as it might be garbage.
        del list[0]
        for s in reversed (self.stats_enabled):
            self.proto.send (s, 'B', 0)
        # Extract asked stats.
        array = numpy.array (list)
        array = array[:, self.stats_items_pos]
        # Cleanup.
        self.stats_enabled = None
        del self.stats_items_pos
        del self.stats_counter
        del self.stats_count
        del self.stats_list
        del self.stats_line
        return array

    def consign (self, w, c):
        """Consign offset."""
        if w == 't':
            self.proto.send ('c', 'hh', c, 0)
        elif w == 'a':
            self.proto.send ('c', 'hh', 0, c)
        else:
            assert w == 'a0'
            self.proto.send ('c', 'h', c)

    def speed (self, w, s):
        """Speed consign."""
        if w == 't':
            self.proto.send ('s', 'bb', s, 0)
        elif w == 'a':
            self.proto.send ('s', 'bb', 0, s)
        else:
            assert w == 'a0'
            self.proto.send ('s', 'b', s)

    def speed_pos (self, w, offset):
        """Speed controlled position consign."""
        if w == 't':
            self.mseq += 1
            self.proto.send ('s', 'LLB', offset, 0, self.mseq)
        elif w == 'a':
            self.mseq += 1
            self.proto.send ('s', 'LLB', 0, offset, self.mseq)
        else:
            assert w == 'a0'
            self.a0seq += 1
            self.proto.send ('s', 'LB', offset, self.a0seq)
        self.wait (self.finished, auto = True)

    def set_pos (self, x = None, y = None, a = None):
        """Set current position."""
        if x is not None:
            self.proto.send ('p', 'BL', ord ('X'),
                    256 * x / self.param['scale'])
        if y is not None:
            self.proto.send ('p', 'BL', ord ('Y'),
                    256 * y / self.param['scale'])
        if a is not None:
            self.proto.send ('p', 'BL', ord ('A'), a * (1 << 24) / 360)

    def goto (self, x, y, backward_ok = False):
        """Go to position."""
        self.mseq += 1
        self.proto.send (backward_ok and 'r' or 'x', 'LLB',
                256 * x / self.param['scale'],
                256 * y / self.param['scale'], self.mseq)
        self.wait (self.finished, auto = True)

    def goto_angle (self, a):
        """Go to angle."""
        self.mseq += 1
        self.proto.send ('x', 'HB', a * (1 << 16) / 360, self.mseq)
        self.wait (self.finished, auto = True)

    def goto_xya (self, x, y, a, backward_ok = False):
        """Go to position, then angle."""
        self.mseq += 1
        self.proto.send (backward_ok and 'r' or 'x', 'LLHB',
                256 * x / self.param['scale'],
                256 * y / self.param['scale'],
                a * (1 << 16) / 360, self.mseq)
        self.wait (self.finished, auto = True)

    def set_simu_pos (self, x, y, a):
        """Set simulated position."""
        self.proto.send ('h', 'BHHH', ord ('X'), x, y,
                math.radians (a) * 1024)

    def register_pos (self, func, interval = 225 / 4):
        """Will call func each time a position is received."""
        self.pos_func = func
        self.proto.register ('X', 'lll', self.handle_pos)
        self.proto.send ('X', 'B', interval)

    def send_param (self):
        p = self.param
        self.proto.send ('p', 'BHH', ord ('p'), p['tkp'] * 256,
                p['akp'] * 256)
        self.proto.send ('p', 'BHH', ord ('i'), p['tki'] * 256,
                p['aki'] * 256)
        self.proto.send ('p', 'BHH', ord ('d'), p['tkd'] * 256,
                p['akd'] * 256)
        self.proto.send ('p', 'BH', ord ('p'), p['a0kp'] * 256)
        self.proto.send ('p', 'BH', ord ('i'), p['a0ki'] * 256)
        self.proto.send ('p', 'BH', ord ('d'), p['a0kd'] * 256)
        self.proto.send ('p', 'BH', ord ('E'), p['E'])
        self.proto.send ('p', 'BH', ord ('I'), p['I'])
        self.proto.send ('p', 'BH', ord ('D'), p['D'])
        self.proto.send ('p', 'BHHH', ord ('b'), p['be'], p['bs'], p['bc'])
        self.proto.send ('p', 'BHH', ord ('a'), p['ta'] * 256,
                p['aa'] * 256)
        self.proto.send ('p', 'BH', ord ('a'), p['a0a'] * 256)
        self.proto.send ('p', 'BBBBB', ord ('s'), p['tsm'], p['asm'],
                p['tss'], p['ass'])
        self.proto.send ('p', 'BBB', ord ('s'), p['a0sm'], p['a0ss'])
        self.proto.send ('p', 'BL', ord ('c'), p['c'] * 256 * 256 * 256)
        self.proto.send ('p', 'BH', ord ('f'), p['f'])
        self.proto.send ('p', 'BH', ord ('l'), p['l'])

    def write_eeprom (self):
        self.proto.send ('p', 'BB', ord ('E'), 1)
        time.sleep (1)
        self.wait (lambda: True)

    def handle_stats (self, stat, *args):
        if self.stats_enabled is not None:
            self.stats_line.extend (args)
            if self.stats_counter == stat:
                self.stats_list.append (self.stats_line)
                self.stats_line = [ ]
                self.stats_count += 1

    def handle_ack (self, mseq, a0seq):
        self.mseq_ack = mseq & 0x7f
        self.a0seq_ack = a0seq & 0x7f
        self.proto.send ('a', 'BB', mseq, a0seq)

    def handle_pos (self, x, y, a):
        x = x / 256 * self.param['scale']
        y = y / 256 * self.param['scale']
        a = a * 360 / (1 << 24)
        self.pos_func (x, y, a)

    def wait (self, cond = None, auto = False):
        if auto and self.async:
            return
        try:
            cond_count = int (cond)
            cond = lambda: self.stats_count > cond_count
        except TypeError:
            pass
        self.proto.wait (cond)

    def finished (self):
        return self.mseq == self.mseq_ack and self.a0seq == self.a0seq_ack

    def free (self):
        self.proto.send ('w')

    def reset (self):
        self.proto.send ('w')
        self.proto.send ('w', 'H', 0)
        self.proto.send ('z')
        self.proto.send ('z')

    def close (self):
        self.reset ()
        self.wait (lambda: True)
        self.proto.file.close ()

    def fileno (self):
        return self.proto.fileno ()
