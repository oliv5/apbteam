import serial
import time
from proto import *
from utils import *

FLASH_MEMORY_HIGH = 0x1fffff
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
        self.__proto = Proto (serial.Serial ('/dev/ttyACM0'), time.time, 0.1)
        self.__memory = list()
        self.__status = False
        self.__addr_init = 0
        self.__addr_end = 0

    def __dump_callback (self, *memory):
        """Callback call on each data reception"""
        for i in range(len (memory)):
            self.__memory.append (memory[i])

    def __flash_status (self, status):
        """Get the flash status."""
        print "Flash activate : ", status
        self.__status = status

    def __flash_log_init (self, *addr):
        """Get the flash log start address."""
        # Don't request the code start
        addr = reverse_tupple (addr)
        self.__addr_init = get_size (addr, 3)

    def __flash_log_end (self, *addr):
        """Get the flash log end address."""
        addr = reverse_tupple (addr)
        self.__addr_end = get_size (addr, 3)

    def dump_memory(self):
        """Dump the flash memory."""
        # Initialise the flash access.
        self.__proto.register ('r', FLASH_BUFFER_SIZE * 'B', self.__dump_callback)
        self.__proto.register ('s', 'b', self.__flash_status)
        self.__proto.register ('i', 3*'B', self.__flash_log_init)
        self.__proto.register ('e', 3*'B', self.__flash_log_end)
        print "Callback registered"
        print "Initialise the Flash memory"
        self.__proto.send ('l', 'b', FLASH_CMD_INIT)
        self.__proto.wait (lambda: True)

        if self.__status == True:
            print  "Dumping from " +  str (self.__addr_init) + " to " + str (self.__addr_end)

            i = self.__addr_init
            while i != self.__addr_end:
                self.__proto.send ('l', 'bI', FLASH_CMD_READ, (i << 8) | FLASH_BUFFER_SIZE)
                i = flash_memory_addr (i + FLASH_BUFFER_SIZE)
            self.__proto.wait (lambda: True)
        else:
            self.__memory = None

    def get_trace (self):
        """Return the traces dumped from the flash memory."""
        return reverse_tupple (self.__memory[4:len (self.__memory)])
