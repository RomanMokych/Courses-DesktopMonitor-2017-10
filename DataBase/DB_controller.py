from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String, ForeignKey, create_engine
from sqlalchemy.orm import relationship, sessionmaker
from sqlalchemy.types import DECIMAL
from sqlalchemy.types import TIMESTAMP
from  sqlalchemy.dialects import mysql


Base = declarative_base()

class Client(Base):
    __tablename__ = 'Client'
    id_client = Column(Integer, primary_key=True, autoincrement=True)
    ip = Column(String(45), unique=True)

    def __init__(self, ip):
        self.ip = ip

class Frame(Base):
    __tablename__ = 'Frame'
    id = Column(Integer, primary_key=True)
    time = Column(DECIMAL)
    client_id = Column(Integer, ForeignKey('Client.id_client'))
    parent = relationship("Client", backref="frame")

    def __init__(self, time):
        self.time = time

class Role(Base):
    __tablename__ = 'Role'
    token = Column(String(45), primary_key=True, unique=True)
    user_name = Column(String(45))

    def __init__(self, role, token):
        self.token = token
        self.user_name = role

class Context:
    def __init__(self, conString):
        self.engine = create_engine(conString,pool_size=20,max_overflow=2000)
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

    def AddRole(self, role, token):
        self.StartSession()
        new_role = Role(role, token)
        self.session.add(new_role)
        self.EndSession()

    def IsClient(self, client_ip):
        self.StartSession()
        clientRes= self.GetClient(client_ip)

        if clientRes is None:
            return False
        else:
            return True

    def GetFramePathes(self, clientid):
        self.StartSession()
        query = "select path from Frame where Frame.client_id ='%d'"%clientid

        selectFrames = self.session.execute(query)

        return selectFrames

    def GetClient(self, ip):
        self.StartSession()
        #query = "select * from Client where ip = '%s'"%ip
        #select_cl = self.session.execute(query)

        select_client = self.session.query(Client).filter_by(ip=ip).first()
        self.EndSession()

        return select_client

    def GetAlLIP(self):
        self.StartSession()
        selIp = self.session.execute("SELECT ip from client")
        self.EndSession()
        return selIp

    def GetRangeFrames(self, startTime, endTime, clientip):
        self.StartSession()
        client_id = self.GetClient(clientip).id_client
        #print(client_id)

        query = "select time from Frame where time BETWEEN %s and %s  and client_id='%d'"%(startTime,endTime, client_id)

        selectFrames = self.session.execute(query)
        list_names = list()
        for row in selectFrames:
            list_names.append(str(row.time))

        self.EndSession()
        return list_names

    def RoleInit(self, token):
        self.StartSession()
        role = self.session.query(Role).filter_by(token = token ).first()
        self.EndSession()
        return role