def get_size (mem, size):
    table = mem[0:size]
    if size == 1:
       return table[0]
    elif size == 2:
        return ((table[0] << 8) | table[1])
    elif size == 3:
        return ((table[0] << 16) | (table[1] << 8) | table[2])
    elif size == 4:
        return ((table[0] << 24) | (table[1] << 16) | (table[2] << 8) | table[3])
