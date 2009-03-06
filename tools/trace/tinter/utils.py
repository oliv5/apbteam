def get_size (mem, size):
    if size == 1:
       return mem.pop()
    elif size == 2:
        return ((mem.pop() << 8) | mem.pop())
    elif size == 3:
        return ((mem.pop() << 16) | (mem.pop() << 8) | mem.pop())
    elif size == 4:
        return ((mem.pop() << 24) | (mem.pop() << 16) | (mem.pop() << 8) | mem.pop())

def reverse_tupple (tupple):
    mem = []
    for i in tupple:
        mem = [i] + mem
    return mem
