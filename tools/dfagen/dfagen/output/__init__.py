from ConfigParser import ConfigParser

class UserConfig:
    def __init__ (self, file):
	if file:
	    f = open (file, 'r')
	    cp = ConfigParser ()
	    cp.readfp (f)
	    f.close ()
	    self.dict = dict (cp.items ('user'))
	    if cp.has_section ('templates'):
		self.templates = dict (cp.items ('templates'))
	    else:
		self.templates = dict ()

    def __getitem__ (self, key):
	return self.dict[key]

    def __contains__ (self, key):
	return key in self.dict

import c
import dot

outputs = dict (
	c = c,
	dot = dot,
	)

def get_output (name):
    return outputs[name];
