from twisted.internet import reactor, protocol
from DataBase.DBcontext import Context
import json
import os
import time
from Server.CommandDescription import *
from Server.config import ServerConf

class ScrPack:
    bData = list()
    klientIp = 0
    size = 0
    countRead = 0
    fstatus = Fstatus.NEW
    posx = 0
    posy = 0

class ServHandler(protocol.Protocol):
    catchFlag = False
    pack = ScrPack()
    role = ""
    curframes = list()
    curPFrames = list(list())
    curVip = ""

    def dataReceived(self, data):
        if self.catchFlag:
            self.CatchDatab(data)
            return

        jmeta = json.loads(data)
        token = jmeta[CmdPr.TOK.value]
        self.role = self.parent.dbConx.RoleInit(token).user_name

        if self.role == CmdPr.CLIENT.value:
            if not self.catchFlag:
                self.PrepareClientRec(jmeta)
                return

        if self.role == CmdPr.VW.value:
            self.ReciveViewer(jmeta)

    def CatchDatab(self, data):
        if self.RecivedClient(data):
            self.parent.SaveData(self.pack)
            self.EraseTmpData()

    def PrepareClientRec(self, jmeta):
        self.catchFlag = True
        self.transport.write(b"00")
        self.pack.klientIp = self.transport.getPeer().host
        self.pack.size = int(jmeta[CmdPr.SIZE.value])
        self.pack.fstatus = int(jmeta[CmdPr.SIZE.value])

        if int(jmeta['status']) == Fstatus.CHANGE.value:
            self.pack.posx = int(jmeta[CmdPr.PX.value])
            self.pack.posy = int(jmeta[CmdPr.PY.value])

    def ReciveViewer(self, jmeta):

        if jmeta['F'] == viewerCommand.GETFRAMESINFO.value:
            (frames, parts) = self.parent.dbConx.GetRangeFrames(jmeta[CmdPr.STIME.value],
                                                                jmeta[CmdPr.ETIME.value],
                                                                jmeta[CmdPr.IP.value])
            self.curframes = frames
            self.curPFrames = parts

            response = {CmdPr.SFUNC.value: ServerCommand.RESFRAMESINFO.value, CmdPr.FCOUNT.value: len(self.curframes)}
            self.Write(response)
            self.curVip = jmeta[CmdPr.IP.value]

        if jmeta[CmdPr.SFUNC.value] == viewerCommand.GETFINFO.value:
            full = self.parent.path + '/' + self.curVip + '/' + self.curframes[0] + ServerConf.IMAGEFORMAT
            size = os.path.getsize(full)

            response = {CmdPr.SFUNC.value: ServerCommand.RESFINFO.value,
                        CmdPr.SIZE.value: size,
                        CmdPr.TIME.value: self.curframes[0],
                        CmdPr.PCOUNT.value: len(self.curPFrames[0])}

            self.Write(response)

        if jmeta[CmdPr.SFUNC.value] == viewerCommand.GETPINFO.value:
            full = self.parent.path + '/' + self.curVip + '/' + str(self.curPFrames[0][0].time) + ServerConf.IMAGEFORMAT
            size = os.path.getsize(full)
            response = {CmdPr.SFUNC.value: ServerCommand.RESPINFO.value,
                        CmdPr.SIZE.value: size,
                        CmdPr.TIME.value: str(self.curPFrames[0][0].time),
                        CmdPr.PX.value: self.curPFrames[0][0].Posx,
                        CmdPr.PY.value: self.curPFrames[0][0].Posy}

            self.Write(response)

        if jmeta[CmdPr.SFUNC.value] == viewerCommand.GETIP.value:
            allIp = self.parent.GetAllIp()
            self.transport.write(allIp.encode('utf-8'))

        if jmeta[CmdPr.SFUNC.value] == viewerCommand.READYF.value:
            self.SendFrame()

        if jmeta[CmdPr.SFUNC.value] == viewerCommand.READYP.value:
            self.SendPart()

    def Write(self, response):
        self.transport.write(json.dumps(response).encode('utf-8'))

    def SendPart(self):
        if len(self.curPFrames) == 0:
            return

        full = self.parent.path + '/' + self.curVip + '/' + str(self.curPFrames[0].pop(0).time) + ServerConf.IMAGEFORMAT
        file = self.parent.bytes_from_file(full)
        self.transport.write(file)

    def SendFrame(self):
        if len(self.curframes) == 0:
            return

        full = self.parent.path+'/'+self.curVip+'/'+self.curframes.pop(0)+ ServerConf.IMAGEFORMAT
        file = self.parent.bytes_from_file(full)
        self.transport.write(file)

    def RecivedClient(self, data):
        self.pack.countRead += len(data)
        self.pack.bData += data

        if self.pack.countRead == self.pack.size:
            self.catchFlag = False
            return True
        return False

    def EraseTmpData(self):
        self.pack.size = 0
        self.pack.bData.clear()
        self.pack.countRead =0

    def connectionMade(self):
        print("connection")

    def connectionLost(self, reason):
        print("connection lost", reason)


class DeskServer:
    def __init__(self, dbConStr, mainPath, port):
        self.factory = protocol.ServerFactory()
        self.factory.protocol = ServHandler
        self.factory.protocol.parent = self

        reactor.listenTCP(port, self.factory)
        self.dbConx = Context(dbConStr)
        self.path = mainPath

    def Run(self):
        reactor.run()

    def SaveData(self, pack):

        if not os.path.isdir(self.path + str("/") + pack.klientIp):
            os.makedirs(self.path + str("/") + pack.klientIp)

        if not self.dbConx.IsClient(pack.klientIp):
            self.dbConx.AddClient(pack.klientIp)

        dtimestr = int(time.time())
        allPass= self.path+str("/")+pack.klientIp + str("/") + str(dtimestr)+ ServerConf.IMAGEFORMAT

        if not os.path.isfile(allPass):
            file = open(allPass, 'wb')
            file.write(bytearray(pack.bData))

        if pack.fstatus == Fstatus.CHANGE.value:
            self.dbConx.AddPart(str(dtimestr), pack.klientIp, pack.posx, pack.posy)

        else:
            self.dbConx.AddFrame(pack.klientIp, dtimestr)

    def bytes_from_file(self, path):
        file = open(path, 'rb')
        byte =file.read()
        return byte

    def GetAllIp(self):
        allIp = self.dbConx.GetAlLIP()
        jsLineIp = []

        for row in allIp:
            jsLineIp.append({CmdPr.IP.value: str(str(row[CmdPr.IP.value]))})

        jspackIp = {CmdPr.SFUNC.value: ServerCommand.RESIP.value, CmdPr.LISTIP.value: jsLineIp}

        return json.dumps(jspackIp)