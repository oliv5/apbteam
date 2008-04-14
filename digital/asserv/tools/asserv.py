import proto, time

class Asserv:

    def __init__ (self, file, **param):
	self.proto = proto.Proto (file, time.time, 0.1)
	self.proto.register ('C', 'HHH',
		lambda *args: self.handle_stats ('C', *args))
	self.proto.register ('Z', 'H',
		lambda *args: self.handle_stats ('Z', *args))
	self.proto.register ('S', 'bbb',
		lambda *args: self.handle_stats ('S', *args))
	self.proto.register ('P', 'hhhhhh',
		lambda *args: self.handle_stats ('P', *args))
	self.proto.register ('W', 'hhh',
		lambda *args: self.handle_stats ('W', *args))
	self.stats_ = None
	self.param = dict (
		tkp = 0, tki = 0, tkd = 0,
		akp = 0, aki = 0, akd = 0,
		a0kp = 0, a0ki = 0, a0kd = 0,
		E = 1023, I = 1023, b = 15000,
		ta = 0, aa = 0, a0a = 0,
		tsm = 0, asm = 0, tss = 0, ass = 0, a0sm = 0, a0ss = 0
		)
	self.param.update (param)
	self.send_param ()

    def stats (self, stats, interval = 1):
	"""Activate stats (given by letter)."""
	# The last occuring stats will increment stats_count, so they have to
	# be in the same order than in asserv program.
	all = 'CZSPW'
	stats = [s for s in all if s in stats]
	for s in stats:
	    self.proto.send (s, 'B', interval)
	self.stats_ = stats
	self.stats_counter = stats[-1]
	self.stats_count = 0
	self.stats_list = [ ]
	self.stats_line = [ ]

    def get_stats (self):
	list = self.stats_list
	# Drop first line as it might be garbage.
	del list[0]
	for s in reversed (self.stats_):
	    self.proto.send (s, 'B', 0)
	# Cleanup.
	self.stats_ = None
	del self.stats_counter
	del self.stats_count
	del self.stats_list
	del self.stats_line
	return list

    def consign (self, w, c):
	"""Consign offset."""
	if w == 't':
	    self.proto.send ('c', 'hh', c, 0)
	elif w == 'a':
	    self.proto.send ('c', 'hh', 0, c)
	else:
	    assert w == 'a0'
	    self.proto.send ('c', 'h', c)

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
	self.proto.send ('p', 'BH', ord ('b'), p['b'])
	self.proto.send ('p', 'BHH', ord ('a'), p['ta'] * 256,
		p['aa'] * 256)
	self.proto.send ('p', 'BH', ord ('a'), p['a0a'] * 256)
	self.proto.send ('p', 'BBBBB', ord ('s'), p['tsm'], p['asm'],
		p['tss'], p['ass'])

    def handle_stats (self, stat, *args):
	if self.stats_ is not None:
	    self.stats_line.extend (args)
	    if self.stats_counter == stat:
		self.stats_list.append (self.stats_line)
		self.stats_line = [ ]
		self.stats_count += 1

    def wait (self, cond = None):
	self.proto.wait (cond)

    def reset (self):
	self.proto.send ('w')
	self.proto.send ('z')
