from twisted.internet import reactor, protocol
from DataBase.DB_controller import Context
import json
import os
import time
from datetime import datetime

class ScrPack:
    bData = list()
    klientIp = 0
    size = 0
    countRead = 0

class ServHandler(protocol.Protocol):
    catchFlag = False
    role = ""

    def __init__(self):
        self.pack = ScrPack()

    def dataReceived(self, data):
        if self.catchFlag == False:
            jmeta = json.loads(data)
            self.ReceivedtAll(jmeta)
            return

        if self.role == 'client':
            if self.RecivedClient(data):
                self.parent.SaveData(self.pack)
                self.EraseTmpData()

        if self.role == 'viewer':
            jmeta = json.loads(data)
            paches = self.parent.dbConx.GetRangeFrames(jmeta['startT'], jmeta['endT'], jmeta['ip'])
            self.sendFrames(paches)

    def ReceivedtAll(self, jmeta):
        token = jmeta['token']
        self.role = self.parent.dbConx.RoleInit(token).user_name

        if self.role is not None:
            self.catchFlag = True

        if self.role == 'client':
            self.transport.write(b"00")
            self.pack.klientIp = self.transport.getPeer().host
            self.pack.size = int(jmeta['size'])

        if self.role == 'viewer':
            allIp = self.parent.GetAllIp()
            self.transport.write(allIp)
            print("send")
            print(allIp)

    def sendFrames(self,paches,client_ip):
        self.transport.write(len(paches))

        for p in paches:
            full = self.parent.path+'/'+client_ip+'/'+p
            self.transport.write(self.parent.bytes_from_file(full))

    def RecivedClient(self, data):
        self.pack.bData += data
        self.pack.countRead += len(data)

        if self.pack.countRead == self.pack.size:
            self.catchFlag = False
            return True
        return False

    def EraseTmpData(self):
        self.pack.size =0
        self.pack.bData.clear()
        self.pack.countRead =0

    def connectionMade(self):
        pass

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

        if self.dbConx.IsClient(pack.klientIp) == False:
            self.dbConx.AddClient(pack.klientIp)

        dtimeStamp = datetime.now()
        dtimestr = int(time.mktime(datetime.now().timetuple()))

        allPass= self.path+str("/")+pack.klientIp + str("/") + str(dtimestr)+'.jpg' #jpg

        print("need to Addd file")
        if not os.path.isfile(allPass):
            file = open(allPass, 'wb')
            file.write(bytearray(pack.bData))
            print("Add file")

        self.dbConx.AddFrame(pack.klientIp, dtimeStamp)

    def bytes_from_file(self,path):
        byte = open(path, 'rb').read()
        return byte




    def GetAllIp(self):
        allIp = self.dbConx.GetAlLIP()
        packIp = b"#"
        for row in allIp:
            packIp += str(row['ip']).encode('unicode-escape') + b"#"

        return packIp
