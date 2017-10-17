from  sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column,Integer,String,ForeignKey,create_engine
from sqlalchemy.orm import relationship,sessionmaker,query
from sqlalchemy.dialects.mysql import DATETIME
Base = declarative_base()

class Client(Base):
    __tablename__ = 'Client'
    id_client = Column(Integer, primary_key=True, autoincrement=True)
    ip = Column(String(45),unique=True)

    def __init__(self,ip):
        self.ip = ip

class Frame(Base):
    __tablename__ = 'frame'
    id = Column(Integer,primary_key=True)
    path = Column(String(50),unique=True)
    time = Column(DATETIME)
    client_id = Column(Integer, ForeignKey('Client.id_client'))
    parent = relationship("Client", backref="frame")

    def __init__(self,path,time):
        self.path = path
        self.time = time

class Context:
    def __init__(self,conString):
        self.engine = create_engine(conString)
        Base.metadata.create_all(self.engine)

    def StartSession(self):
        Session = sessionmaker(bind=self.engine)
        self.session = Session()

    def EndSession(self):
        self.session.commit()

    def AddClient(self,ip):
        self.StartSession()
        new_client = Client(ip=ip)
        self.session.add(new_client)
        self.EndSession()

    def GetClient(self,ip):
        self.StartSession()
        select_client = self.session.query(Client).filter_by(ip=ip).first()
        self.EndSession()
        return select_client

    def AddFrame(self,clientIp,path,time):
        self.StartSession()
        new_frame = Frame(path,time)
        select_client = self.GetClient(clientIp)
        new_frame.parent = select_client
        self.session.commit()
        self.EndSession()

    def GetFramePathes(self,clientid):
        self.StartSession()
        query = "select path from Frame where Frame.client_id ='%d'"%clientid

        selectFrames = self.session.execute(query)

        return selectFrames

    def GetRangeFrames(self,startTime,endTime,clientId):
        self.StartSession()
        convDateEnd =  "STR_TO_DATE('"+startTime+"','%Y-%m-%d %H:%i:%s')"
        convDateStart = "STR_TO_DATE('"+endTime +"','%Y-%m-%d %H:%i:%s')"
        query = "select path from Frame where Frame.time BETWEEN %s and %s and Frame.client_id='%d'"%(convDateStart,convDateEnd,clientId)
        print(query)
        selectFrames = self.session.execute(query)

        return selectFrames



