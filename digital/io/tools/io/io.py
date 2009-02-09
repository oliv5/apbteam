import proto, time

class Io:

    def __init__ (self, file, time = time.time, **param):
        self.proto = proto.Proto (file, time, 0.1)
        self.async = False
        self.param = param
        self.send_param ()

    def send_param (self):
        p = self.param
        for i, t in enumerate (p['trap']):
            self.proto.send ('t', 'BBB', i, t[0], t[1])
        for i, t in enumerate (p['sharp_threshold']):
            self.proto.send ('h', 'BHH', i, t[0], t[1])

    def write_eeprom (self):
        self.proto.send ('p', 'BB', ord ('E'), ord ('s'))
        time.sleep (1)
        self.proto.wait (lambda: True)

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

