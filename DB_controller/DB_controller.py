from  sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column,Integer,String,ForeignKey,create_engine
from sqlalchemy.orm import relationship,sessionmaker,query

Base = declarative_base()
class Klient(Base):
    __tablename__ = 'Klient'
    ip = Column(String,primary_key=True)
    def __init__(self,ip):
        self.ip= ip

class Frame(Base):
    __tablename__ = 'frame'
    path = Column(String, primary_key=True)
    time = Column(String)
    klient_ip = Column(Integer, ForeignKey('Klient.ip'))
    parent = relationship("Klient", backref="frames")

    def __init__(self,path,time):
        self.path = path
        self.time = time

class Context:
    def __init__(self,db_path):
        self.engine = create_engine("sqlite:///"+db_path)
        Base.metadata.create_all(self.engine)

    def StartSession(self):
        Session = sessionmaker(bind=self.engine)
        self.session = Session()

    def EndSession(self):
        self.session.commit()

    def AddKlient(self,ip):
        Session = sessionmaker(bind=self.engine)
        session = Session()
        new_klient = Klient(ip)
        session.add(new_klient)
        session.commit()

    def AddFrame(self,klientIp,path,time):
        self.StartSession()
        new_frame = Frame(path,time)
        select_klient = self.session.query(Klient).filter_by(ip='test_token').first()
        new_frame.parent = select_klient
        self.session.commit()
        self.EndSession()

    def GetFrames(self,klient_ip):
        self.StartSession()
        selectFrames = self.session.query(Frame).filter_by(klient_ip=klient_ip).first()
        self.EndSession()
        return selectFrames

