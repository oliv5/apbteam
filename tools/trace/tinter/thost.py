import serial
import time
import sys
from proto import *
from utils import *

FLASH_MEMORY_HIGH = 0x1fffff
FLASH_PAGE = 0x80000
FLASH_BUFFER_SIZE = 128
FLASH_CMD_INIT = 0
FLASH_CMD_READ = 1

def flash_memory_addr (val):
    return (val & FLASH_MEMORY_HIGH)

def log (x):
    print x

class THost:
    """Class to connect to the flash memory."""
    def __init__(self):
        self.__proto = Proto (serial.Serial ('/dev/ttyUSB0'), time.time, 0.1)
        self.__memory = list()
        self.__traces = []

    def __dump_callback (self, *memory):
        """Callback call on each data reception"""
        for i in range(len (memory)):
            self.__memory.append (memory[i])
        sys.stderr.write (".")

    def __trace_present (self, val):
        """Get the trace value and the first byte following."""
        self.__traces.append (val)

    def trace_list (self):
        """Read the flash memory and return the addresses of the traces
        present with the trace value in a array."""
        self.__proto.register ('r', 'B', self.__trace_present)
        self.__proto.send ('l', 'b', FLASH_CMD_INIT)
        self.__proto.send ('l', 4*'b', FLASH_CMD_READ, 0x0, 0x0, 0x0)
        self.__proto.send ('l', 4*'b', FLASH_CMD_READ, 0x8, 0x0, 0x0)
        self.__proto.send ('l', 4*'b', FLASH_CMD_READ, 0x10, 0x0, 0x0)
        self.__proto.send ('l', 4*'b', FLASH_CMD_READ, 0x18, 0x0, 0x0)
        self.__proto.wait (lambda: True)

        return self.__traces

    def dump_memory(self, val):
        """Dump the flash memory."""
        # Initialise the flash access.
        self.__proto.register ('r', FLASH_BUFFER_SIZE * 'B',
                self.__dump_callback)
        self.__proto.register ('r', 'B', self.__trace_present)

        i = 0
        self.__traces.append (0x100)
        while self.__traces[0] != val and i < FLASH_MEMORY_HIGH:
            self.__traces = []
            addr = i >> 16
            self.__proto.send ('l', 4*'b', FLASH_CMD_READ, addr, 0, 0)
            self.__proto.wait (lambda: True)
            i += FLASH_PAGE
        i -= FLASH_PAGE

        start_addr = i
        end_addr = start_addr + FLASH_PAGE
        print "Dump memory from address : " +  hex(start_addr) + " to " + \
                hex(end_addr)

        for i in range (start_addr, end_addr, FLASH_BUFFER_SIZE):
            self.__proto.send ('l', 'bI', FLASH_CMD_READ, (i << 8) | \
                    FLASH_BUFFER_SIZE)
        self.__proto.wait (lambda: True)
        sys.stderr.write ("\nDump ended\n")

    def get_trace (self):
        """Return the traces dumped from the flash memory."""
        return self.__memory[1:]
