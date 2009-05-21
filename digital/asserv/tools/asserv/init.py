"""Default parameters for asserv."""
host = dict (
        scale = 0.0395840674352314, f = 0xdd1,
        tkp = 1, tkd = 16,
        ta = 0.75, tsm = 0x30, tss = 0x10,
        akp = 2, akd = 16,
        aa = 0.25, asm = 0x20, ass = 0x10,
        a0kp = 4,
        a0a = 0.5, a0sm = 0x0c, a0ss = 0x05,
        a1kp = 2,
        a1a = 16, a1sm = 0x46, a1ss = 0x10,
        E = 0x3ff, D = 0x1ff,
        )
target = dict (
        scale = 0.0413530725332892, f = 0xcf7,
        c = float (0x0100b9e4) / (1 << 24),
        tkp = 1, tkd = 16,
        ta = 0.75, tsm = 0x30, tss = 0x10,
        akp = 2, akd = 16,
        aa = 0.25, asm = 0x20, ass = 0x10,
        a0kp = 4,
        a0a = 0.5, a0sm = 0x0c, a0ss = 0x05,
        a0be = 256, a0bs = 0x04, a0bc = 10,
        a1kp = 2,
        a1a = 16, a1sm = 0x46, a1ss = 0x10,
        a1be = 256, a1bs = 0x0c, a1bc = 10,
        E = 0x3ff, D = 0x1ff,
        w = 0x08,
        )
