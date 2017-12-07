from enum import Enum

class Cmd_property(Enum):

    FUNCTION = 'F'         # This symbol denotes the function
    TOKEN = 'token'
    CLIENT = 'client'
    VIEWER = 'viewer'
    SIZE = 'size'
    STATUS = 'status'
    LEFTTOPX = 'posx'
    LEFTTOPY = 'posy'
    IP = 'ip'
    STARTTIME = 'startT'
    ENDTIME = 'endT'
    FRAMECOUNT = 'Fcount'
    TIME = 'time'
    PARTCOUNT = 'partcount'
    IPLIST = 'list_ip'

class ServerCommand(Enum):
    RESIP = 0
    RESFRAMEINFO = 1
    RESPARTINFO = 2
    RESFRAMESINFO = 3

class viewerCommand(Enum):
    GETFRAMESINFO = 0
    GETFRAMEINFO = 1
    GETPARTINFO = 2
    GETIP = 3
    READYFRAME = 4
    READYPART = 5

class Framestatus(Enum):
    NEW = 0
    CHANGE = 1
    BENCH = 2

