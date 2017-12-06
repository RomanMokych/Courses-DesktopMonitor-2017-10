from DataBase.DB_controller import *
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from Server.config import DBconfig

class Context:
    def __init__(self, conString):
        self.engine = create_engine(conString, pool_size=DBconfig.DBMAXPOOL, max_overflow=DBconfig.DBMAXOVERFLOW)
        Base.metadata.create_all(self.engine)

    def StartSession(self):
        Session = sessionmaker(bind=self.engine)
        self.session = Session()

    def EndSession(self):
        self.session.commit()

    def AddClient(self, ip):
        self.StartSession()
        new_client = Client(ip=ip)
        self.session.add(new_client)
        self.EndSession()

    def AddFrame(self, clientIp, time):
        self.StartSession()
        new_frame = Frame(time)
        select_client = self.GetClient(clientIp)
        new_frame.parent = select_client
        self.session.add(new_frame)
        self.EndSession()

    def AddPart(self, time, ip, x, y):
        self.StartSession()
        lastFrameId = self.GetLastFrame(ip)
        new_part = FramePart(time, lastFrameId.id, x, y)

        self.session.add(new_part)
        self.EndSession()

    def AddRole(self, role, token):
        self.StartSession()
        new_role = Role(role, token)
        self.session.add(new_role)
        self.EndSession()

    def IsClient(self, client_ip):
        self.StartSession()
        clientres = self.GetClient(client_ip)

        if clientres is None:
            return False
        else:
            return True

    def GetClient(self, ip):
        self.StartSession()

        select_client = self.session.query(Client).filter_by(ip=ip).first()
        self.EndSession()

        return select_client

    def GetLastFrame(self, ip):
        self.StartSession()
        selectclient = self.GetClient(ip)
        query = "select * from Frame where time =(select MAX(time)from Frame) and   client_id = '%s'" % selectclient.id
        maxtime = self.session.execute(query)
        self.EndSession()
        return maxtime.fetchone()

    def GetAlLIP(self):
        self.StartSession()
        selip = self.session.execute("SELECT ip from client")
        self.EndSession()
        return selip

    def GetRangeFrames(self, startTime, endTime, clientip):
        self.StartSession()
        client = self.GetClient(clientip)

        query = "select * from Frame where time BETWEEN %s and %s  and client_id='%d'" % (startTime, endTime, client.id)
        i = 0
        selectframes = self.session.execute(query)
        list_fnames = list()
        list_pnames = list(list())

        for row in selectframes:
            list_fnames.append(str(row.time))
            querypart = "select time,Posx,Posy from FramePart where frame_id = '%d'" % row.id
            select_parts = self.session.execute(querypart)

            list_pnames.append(list())

            for prow in select_parts:
                list_pnames[i].append(prow)

            i += 1

        self.EndSession()
        return (list_fnames, list_pnames)

    def RoleInit(self, token):
        self.StartSession()
        role = self.session.query(Role).filter_by(token=token).first()
        self.EndSession()
        return role
