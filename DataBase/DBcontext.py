from DataBase.DB_controller import *
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from Server.config import DBconfig

class Context:
    def __init__(self, conString):
        self.engine = create_engine(conString, pool_size=DBconfig.DBMAXPOOL, max_overflow=DBconfig.DBMAXOVERFLOW)
        Base.metadata.create_all(self.engine)

    def startSession(self):
        Session = sessionmaker(bind=self.engine)
        self.session = Session()

    def endSession(self):
        self.session.commit()

    def add_client(self, ip):
        self.startSession()
        new_client = Client(ip=ip)
        self.session.add(new_client)
        self.endSession()

    def add_frame(self, clientIp, time):
        self.startSession()
        new_frame = Frame(time)
        select_client = self.getClient(clientIp)
        new_frame.parent = select_client
        self.session.add(new_frame)
        self.endSession()

    def add_part(self, time, ip, x, y):
        self.startSession()
        lastFrameId = self.get_last_frame(ip)
        new_part = FramePart(time, lastFrameId.id, x, y)

        self.session.add(new_part)
        self.endSession()

    def add_role(self, role, token):
        self.startSession()
        new_role = Role(role, token)
        self.session.add(new_role)
        self.endSession()

    def is_client(self, client_ip):
        self.startSession()
        clientres = self.getClient(client_ip)

        if clientres is None:
            return False
        else:
            return True

    def getClient(self, ip):
        self.startSession()

        select_client = self.session.query(Client).filter_by(ip=ip).first()
        self.endSession()

        return select_client

    def get_last_frame(self, ip):
        self.startSession()
        selectclient = self.getClient(ip)
        query = "select * from Frame where time =(select MAX(time)from Frame) and   client_id = '%s'" % selectclient.id
        maxtime = self.session.execute(query)
        self.endSession()
        return maxtime.fetchone()

    def get_all_ip(self):
        self.startSession()
        selip = self.session.execute("SELECT ip from client")
        self.endSession()
        return selip

    def get_range_frames(self, startTime, endTime, clientip):
        self.startSession()
        client = self.getClient(clientip)

        query = "select * from Frame where time BETWEEN %s and %s  and client_id='%d'" % (startTime, endTime, client.id)
        i = 0
        selectframes = self.session.execute(query)
        list_fnames = list()
        list_pnames = list(list())

        for row in selectframes:
            list_fnames.append(str(row.time))
            querypart = "select time,lefttop_x,lefttop_y from FramePart where frame_id = '%d'" % row.id
            select_parts = self.session.execute(querypart)

            list_pnames.append(list())

            for prow in select_parts:
                list_pnames[i].append(prow)

            i += 1

        self.endSession()
        return (list_fnames, list_pnames)

    def role_init(self, token):
        self.startSession()
        role = self.session.query(Role).filter_by(token=token).first()
        self.endSession()
        return role
