from enum import Enum
class ServerCommand(Enum):
    RESIP = 0
    RESFINFO = 1
    RESPINFO = 2
    RESFRAMESINFO = 3

class viewerCommand(Enum):
    GETFRAMESINFO = 0
    GETFINFO = 1
    GETPINFO = 2
    GETIP = 3
    READYF = 4
    READYP = 5

class Fstatus(Enum):
    NEW = 0
    CHANGE = 1
    BENCH = 2
