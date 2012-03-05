"""Default parameters for asserv."""
target_marcel = dict (
        a0kp = 4,
        a0a = 16, a0sm = 0x60, a0ss = 0x10,
        a0be = 256, a0bs = 0x18, a0bc = 5,
        a1kp = 4,
        a1a = 16, a1sm = 0x60, a1ss = 0x10,
        a1be = 256, a1bs = 0x18, a1bc = 5,
        E = 0x3ff, D = 0x1ff,
        )
target_robospierre = dict (
        a0kp = 8, a0kd = 1,
        a0a = 2, a0sm = 0x60, a0ss = 0x10,
        a0be = 32, a0bs = 0x08, a0bc = 125,
        a1kp = 4,
        a1a = 0.5, a1sm = 0x30, a1ss = 0x08,
        a1be = 64, a1bs = 0x08, a1bc = 5,
        E = 0x3ff, D = 0x1ff,
        )
target = {
        'marcel': target_marcel,
        'robospierre': target_robospierre,
        }

host = target
