"""Default parameters for asserv."""
target_marcel = dict (
        a0kp = 4,
        a0a = 16, a0sm = 0x60, a0ss = 0x10,
        a0be = 256, a0bs = 0x18, a0bc = 5,
        a1kp = 4,
        a1a = 16, a1sm = 0x60, a1ss = 0x10,
        a1be = 256, a1bs = 0x18, a1bc = 5,
        E = 0x3ff, D = 0x1ff,
        w = 0x03,
        )
target = {
        'marcel': target_marcel,
        'robospierre': target_marcel,
        }

host = target
