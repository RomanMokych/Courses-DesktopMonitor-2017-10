from twisted.internet import reactor, protocol
from DataBase.DB_controller import Context
import json
import os
import time

class ScrPack:
    bData = list()
    klientIp = 0
    size = 0
    countRead = 0

class ServHandler(protocol.Protocol):
    catchFlag = False
    pack = ScrPack()
    role = ""

    curframes = list()
    curVip = ""

    def dataReceived(self, data):
        if self.catchFlag == True:
            self.catchDatab(data)
            return

        jmeta = json.loads(data)
        token = jmeta['token']
        self.role = self.parent.dbConx.RoleInit(token).user_name

        if self.role == 'client':
            if self.catchFlag == False:
                self.PrepareClientRec(jmeta)
                return

        if self.role == 'viewer':
            self.ReciveViewer(jmeta)

    def catchDatab(self, data):
        if self.RecivedClient(data):
            self.parent.SaveData(self.pack)
            self.EraseTmpData()

    def PrepareClientRec(self, jmeta):
        self.catchFlag = True
        self.transport.write(b"00")
        self.pack.klientIp = self.transport.getPeer().host
        self.pack.size = int(jmeta['size'])


    def ReciveViewer(self, jmeta):

        if jmeta['F'] == 'getframes':
            paches = self.parent.dbConx.GetRangeFrames(jmeta['startT'], jmeta['endT'], jmeta['ip'])
            response = {'F': 'FRinfo', 'count': len(paches)}
            self.transport.write(json.dumps(response).encode('utf-8'))
            self.curframes = paches
            self.curVip = jmeta['ip']

        if jmeta['F']== "getip":
            allIp = self.parent.GetAllIp()
            self.transport.write(allIp.encode('utf-8'))

        if jmeta['F'] == "readyF":
            self.sendFrame()

        if jmeta['F'] == "gettime":
            jtime = self.getJtime()
            response = json.dumps(jtime).encode('utf-8')
            self.transport.write(response)

    def getJtime(self):
        jspackIp = {'F': 'resTime', 'timeList': self.curframes}
        return jspackIp

    def sendFrame(self):
        if len(self.curframes) == 0:
            return

        full = self.parent.path+'/'+self.curVip+'/'+self.curframes.pop(0)+".PNG"
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

        dtimestr = int(time.time())
        allPass= self.path+str("/")+pack.klientIp + str("/") + str(dtimestr)+'.PNG'

        if not os.path.isfile(allPass):
            file = open(allPass, 'wb')
            file.write(bytearray(pack.bData))
            print("Add file")

        self.dbConx.AddFrame(pack.klientIp, dtimestr)

    def bytes_from_file(self,path):
        file = open(path,'rb')
        byte =file.read()
        return byte

    def GetAllIp(self):
        allIp = self.dbConx.GetAlLIP()
        jsLineIp = []

        for row in allIp:
            jsLineIp.append({'ip': str(str(row['ip']))})

        jspackIp = {'F': 'IpResult', 'list_ip': jsLineIp}

        print(json.dumps(jspackIp))
        return json.dumps(jspackIp)
