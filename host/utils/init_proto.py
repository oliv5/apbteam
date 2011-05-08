"""Helper to create a Proto instance."""
import proto.popen_io
import serial
import optparse

def init_proto (default_robot, proto_class, init_module = None, init = None):
    """Helper to create a Proto instance from command line arguments."""
    if init_module is None and init is None:
        init = { }
    # Parse arguments.
    parser = optparse.OptionParser (
            usage = "usage: %prog [options] TTY|! PROGRAM...",
            description = "TTY is a device name (example: %prog "
            "/dev/ttyUSB0), PROGRAM is a host program with its arguments "
            "(example: %prog -- ! ../src/board.host board_arg).")
    if init_module:
        parser.add_option ('-r', '--robot', help = "use specified robot",
                metavar = 'NAME', default = default_robot)
    (options, args) = parser.parse_args ()
    if init_module and options.robot is None:
        parser.error ("no robot specified")
    if not args:
        parser.error ("not enough arguments")
    # Create parameters.
    if args[0] == '!':
        io = proto.popen_io.PopenIO (args[1:])
        if init_module:
            init = init_module.host[options.robot]
    else:
        if len (args) != 1:
            parser.error ("too many arguments after device")
        io = serial.Serial (args[0])
        if init_module:
            init = init_module.target[options.robot]
    return proto_class (io, **init)

