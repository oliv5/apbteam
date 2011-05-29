"""Default parameters for asserv."""
host_marcel = dict (
        scale = 0.0395840674352314, f = 0xdd1,
        tkp = 1, tkd = 16,
        ta = 0.75, tsm = 0x20, tss = 0x10,
        akp = 2, akd = 16,
        aa = 0.25, asm = 0x20, ass = 0x10,
        a0kp = 4,
        a0a = 4, a0sm = 0x60, a0ss = 0x10,
        a1kp = 2,
        a1a = 16, a1sm = 0x46, a1ss = 0x10,
        E = 0x3ff, D = 0x1ff,
        l = 0x1000,
        )
host = {
        'giboulee': host_marcel,
        'marcel': host_marcel,
        'robospierre': host_marcel,
        }
target_marcel = dict (
        scale = 0.0415178942124, f = 0xcef,
        c = float (0x00ffbabf) / (1 << 24),
        tkp = 1, tkd = 16,
        ta = 0.75, tsm = 0x20, tss = 0x10,
        akp = 2, akd = 16,
        aa = 0.25, asm = 0x20, ass = 0x10,
        a0kp = 4, a0kd = 16,
        a0a = 4, a0sm = 0x60, a0ss = 0x10,
        a0be = 256, a0bs = 0x04, a0bc = 10,
        a1kp = 2,
        a1a = 16, a1sm = 0x46, a1ss = 0x10,
        a1be = 256, a1bs = 0x0c, a1bc = 10,
        E = 0x3ff, D = 0x1ff,
        l = 0x1000,
        w = 0x09,
        )
target_robospierre = dict (
        scale = 0.0317975134344, f = 0x134e,
        c = float (0xffa897) / (1 << 24),
        tkp = 1, tkd = 16,
        ta = 0.75, tsm = 0x60, tss = 0x10,
        tbe = 256, tbs = 0x08, tbc = 40,
        akp = 2, akd = 16,
        aa = 0.5, asm = 0x60, ass = 0x10,
        abe = 128, abs = 0x08, abc = 40,
        E = 0x3ff, D = 0x1ff,
        l = 0x1000,
        w = 0x00,
        )
target = {
        'giboulee': target_marcel,
        'marcel': target_marcel,
        'robospierre': target_robospierre,
        }
