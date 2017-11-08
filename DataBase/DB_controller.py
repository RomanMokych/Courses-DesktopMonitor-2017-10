from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String, ForeignKey, create_engine
from sqlalchemy.orm import relationship, sessionmaker
from sqlalchemy.types import TIMESTAMP

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
    time = Column(TIMESTAMP)
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
        self.engine = create_engine(conString)
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
        self.session.commit()
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
        select_client = self.session.query(Client).filter_by(ip=ip).first()
        self.EndSession()
        return select_client

    def GetRangeFrames(self, startTime, endTime, clientId):
        self.StartSession()
        convDateEnd = "STR_TO_DATE('" + startTime + "','%Y-%m-%d %H:%i:%s')"
        convDateStart = "STR_TO_DATE('" + endTime + "','%Y-%m-%d %H:%i:%s')"
        query = "select path from Frame where Frame.time BETWEEN %s and %s and Frame.client_id='%d'"%(convDateStart, convDateEnd, clientId)
        print(query)
        selectFrames = self.session.execute(query)

        return selectFrames

    def RoleInit(self, token):
        self.StartSession()
        role = self.session.query(Role).filter_by(token = token ).first()
        self.EndSession()
        return role