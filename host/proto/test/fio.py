
class IO:
    def __init__ (self, fin = None, fout = None):
        if fin is None:
            import sys, tty
            self.fin = sys.stdin
            self.fout = sys.stdout
            tty.setcbreak (sys.stdin.fileno ())
        else:
            self.fin = fin
            self.fout = fout

    def read (self, *args):
        buf = self.fin.read (*args).replace ('\n', '\r')
        return buf

    def write (self, *args):
        return self.fout.write (*[i.replace ('\r', '\n') for i in args])

    def fileno (self):
        return self.fin.fileno ()

