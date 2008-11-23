# Define the trace Param.
class TraceParam:
    def __init__ (self, name, length):
        self.__name = name

        if (length == 0) or (length == 3) or (length > 4):
	    self.__length = 0 
            raise Exception ("Length not permitted")
        else:
            self.__length = length

    def name (self):
        return self.__name

    def length (self):
        return self.__length

# Defines the Events of the trace module.
class TraceEvent:
    def __init__(self, name = ''):
	self.__name = name
        self.__param_list = list()
        self.__string = ""

    def name (self):
	return self.__name

    def param_add (self, name, length):
        param = TraceParam (name, length)
        self.__param_list.append(param)
    
    def param_get (self, pos):
        if pos <= len (self.__param_list):
            return self.__param_list[pos]

    def param_nb (self):
        return len(self.__param_list)

    def string_set (self, string = ""):
        self.__string = string

    def string_get (self):
        return self.__string
