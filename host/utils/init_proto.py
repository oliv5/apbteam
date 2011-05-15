"""Helper to create a Proto instance."""
import proto.popen_io
import serial
import optparse

class InitProto:
    """Helper to create a Proto instance from command line arguments."""

    def __init__ (self, default_robot, proto_class, init_module = None,
            init = None):
        """Initialise helper."""
        if init_module is None and init is None:
            init = { }
        self.proto_class = proto_class
        self.init_module = init_module
        self.init = init
        # Prepare parser.
        self.parser = optparse.OptionParser (
                usage = "usage: %prog [options] TTY|! PROGRAM...",
                description = "TTY is a device name (example: %prog "
                "/dev/ttyUSB0), PROGRAM is a host program with its arguments "
                "(example: %prog -- ! ../src/board.host board_arg).")
        if init_module:
            self.parser.add_option ('-r', '--robot',
                    help = "use specified robot",
                    metavar = 'NAME', default = default_robot)

    def parse_args (self):
        """Parse command line."""
        (self.options, self.args) = self.parser.parse_args ()
        if self.init_module and self.options.robot is None:
            self.parser.error ("no robot specified")

    def get_proto (self):
        """Return the Proto instance."""
        if not self.args:
            self.parser.error ("not enough arguments")
        # Create parameters.
        if self.args[0] == '!':
            io = proto.popen_io.PopenIO (self.args[1:])
            if self.init_module:
                self.init = self.init_module.host[self.options.robot]
        else:
            if len (self.args) != 1:
                self.parser.error ("too many arguments after device")
            io = serial.Serial (self.args[0])
            if self.init_module:
                self.init = self.init_module.target[self.options.robot]
        return self.proto_class (io, **self.init)

def init_proto (default_robot, proto_class, init_module = None, init = None):
    """Helper to create a Proto instance from command line arguments."""
    ip = InitProto (default_robot, proto_class, init_module, init)
    ip.parse_args ()
    return ip.get_proto ()

