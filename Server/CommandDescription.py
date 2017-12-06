from enum import Enum

class CmdPr(Enum):

    SFUNC = 'F'         # This symbol denotes the function
    TOK = 'token'
    CLIENT = 'client'
    VW = 'viewer'
    SIZE = 'size'
    STAT = 'status'
    PX = 'posx'
    PY = 'posy'
    IP = 'ip'
    STIME = 'startT'
    ETIME = 'endT'
    FCOUNT = 'Fcount'
    TIME = 'time'
    PCOUNT = 'partcount'
    LISTIP = 'list_ip'

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

