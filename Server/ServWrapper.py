from twisted.internet import reactor, protocol
from DataBase.DBcontext import Context
import json
import os
import time
from Server.CommandDescription import *
from Server.config import ServerConf

class Frame:
    data_buff = list()
    klientIp = 0
    frame_size = 0
    count_read_bytes = 0
    frame_status = Framestatus.NEW
    lefttop_x = 0
    lefttop_y = 0

class ServHandler(protocol.Protocol):
    catch_flag = False
    pack = Frame()
    role = ""
    current_frames = list()
    current_parts = list(list())
    current_ip = ""

    def dataReceived(self, data):
        if self.catch_flag:
            self.catch_bytes(data)
            return

        meta_data = json.loads(data)
        token = meta_data[Cmd_property.TOKEN.value]
        self.role = self.parent.db_conx.role_init(token).user_name

        if self.role == Cmd_property.CLIENT.value:
            if not self.catch_flag:
                self.prepare_client(meta_data)
                return

        if self.role == Cmd_property.VIEWER.value:
            self.recive_viewer(meta_data)

    def catch_bytes(self, data):
        if self.recive_client(data):
            self.parent.save_data(self.pack)
            self.erase_tmp_data()

    def prepare_client(self, meta_data):
        self.catch_flag = True
        self.transport.write(b"00")
        self.pack.klientIp = self.transport.getPeer().host
        self.pack.frame_size = int(meta_data[Cmd_property.SIZE.value])
        self.pack.frame_status = int(meta_data[Cmd_property.STATUS.value])

        if int(meta_data[Cmd_property.STATUS.value]) == Framestatus.CHANGE.value:
            self.pack.lefttop_x = int(meta_data[Cmd_property.LEFTTOPX.value])
            self.pack.lefttop_y = int(meta_data[Cmd_property.LEFTTOPY.value])

    def recive_viewer(self, meta_data):

        if meta_data[Cmd_property.FUNCTION.value] == viewerCommand.GETFRAMESINFO.value:
            (frames, parts) = self.parent.db_conx.get_range_frames(meta_data[Cmd_property.STARTTIME.value],
                                                                meta_data[Cmd_property.ENDTIME.value],
                                                                meta_data[Cmd_property.IP.value])
            self.current_frames = frames
            self.current_parts = parts

            response = {Cmd_property.FUNCTION.value: ServerCommand.RESFRAMESINFO.value, Cmd_property.FRAMECOUNT.value: len(self.current_frames)}
            self.write(response)
            self.current_ip = meta_data[Cmd_property.IP.value]

        if meta_data[Cmd_property.FUNCTION.value] == viewerCommand.GETFRAMEINFO.value:
            full = self.parent.path + '/' + self.current_ip + '/' + self.current_frames[0] + ServerConf.IMAGEFORMAT
            self.frame_size = os.path.getsize(full)

            response = {Cmd_property.FUNCTION.value: ServerCommand.RESFRAMEINFO.value,
                        Cmd_property.SIZE.value: self.frame_size,
                        Cmd_property.TIME.value: self.current_frames[0],
                        Cmd_property.PARTCOUNT.value: len(self.current_parts[0])}

            self.write(response)

        if meta_data[Cmd_property.FUNCTION.value] == viewerCommand.GETPARTINFO.value:
            full = self.parent.path + '/' + self.current_ip + '/' + str(self.current_parts[0][0].time) + ServerConf.IMAGEFORMAT
            self.frame_size = os.path.getsize(full)
            response = {Cmd_property.FUNCTION.value: ServerCommand.RESPARTINFO.value,
                        Cmd_property.SIZE.value: self.frame_size,
                        Cmd_property.TIME.value: str(self.current_parts[0][0].time),
                        Cmd_property.LEFTTOPX.value: self.current_parts[0][0].lefttop_x,
                        Cmd_property.LEFTTOPY.value: self.current_parts[0][0].lefttop_y}

            self.write(response)

        if meta_data[Cmd_property.FUNCTION.value] == viewerCommand.GETIP.value:
            allIp = self.parent.get_all_ip()
            self.transport.write(allIp.encode('utf-8'))

        if meta_data[Cmd_property.FUNCTION.value] == viewerCommand.READYFRAME.value:
            self.send_frame()

        if meta_data[Cmd_property.FUNCTION.value] == viewerCommand.READYPART.value:
            self.send_part()

    def write(self, response):
        self.transport.write(json.dumps(response).encode('utf-8'))

    def send_part(self):
        if len(self.current_parts) == 0:
            return

        full = self.parent.path + '/' + self.current_ip + '/' + str(self.current_parts[0].pop(0).time) + ServerConf.IMAGEFORMAT
        file = self.parent.bytes_from_file(full)
        self.transport.write(file)

    def send_frame(self):
        if len(self.current_frames) == 0:
            return

        full = self.parent.path+'/'+self.current_ip+'/'+self.current_frames.pop(0) + ServerConf.IMAGEFORMAT
        file = self.parent.bytes_from_file(full)
        self.transport.write(file)

    def recive_client(self, data):
        self.pack.count_read_bytes += len(data)
        self.pack.data_buff += data

        if self.pack.count_read_bytes == self.pack.frame_size:
            self.catch_flag = False
            return True
        return False

    def erase_tmp_data(self):
        self.pack.frame_size = 0
        self.pack.data_buff.clear()
        self.pack.count_read_bytes = 0

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
        self.db_conx = Context(dbConStr)
        self.path = mainPath

    def run(self):
        reactor.run()

    def save_data(self, pack):

        if not os.path.isdir(self.path + str("/") + pack.klientIp):
            os.makedirs(self.path + str("/") + pack.klientIp)

        if not self.db_conx.is_client(pack.klientIp):
            self.db_conx.add_client(pack.klientIp)

        dtimestr = int(time.time())
        allPass= self.path+str("/")+pack.klientIp + str("/") + str(dtimestr)+ ServerConf.IMAGEFORMAT

        if not os.path.isfile(allPass):
            file = open(allPass, 'wb')
            file.write(bytearray(pack.data_buff))

        if pack.frame_status == Framestatus.CHANGE.value:
            self.db_conx.add_part(str(dtimestr), pack.klientIp, pack.lefttop_x, pack.lefttop_y)

        else:
            self.db_conx.add_frame(pack.klientIp, dtimestr)

    def bytes_from_file(self, path):
        file = open(path, 'rb')
        byte =file.read()
        return byte

    def get_all_ip(self):
        allIp = self.db_conx.get_all_ip()
        jsLineIp = []

        for row in allIp:
            jsLineIp.append({Cmd_property.IP.value: str(str(row[Cmd_property.IP.value]))})

        jspackIp = {Cmd_property.FUNCTION.value: ServerCommand.RESIP.value, Cmd_property.IPLIST.value: jsLineIp}

        return json.dumps(jspackIp)