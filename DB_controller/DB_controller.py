from  sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column,Integer,String,MetaData,ForeignKey,create_engine
from sqlalchemy.orm import relationship,sessionmaker

Base = declarative_base()


class Klient(Base):
    __tablename__ = 'Klient'
    id = Column(Integer,primary_key=True)


class Frame(Base):
    __tablename__ = 'frame'
    id = Column(Integer, primary_key=True)
    path = Column(String)
    klient_id = Column(Integer, ForeignKey('Klient.id'))
    Frame = relationship("Klient", backref="frames")

    def __init__(self,path):
        self.path = path


class Context:
    def __init__(self,db_path):
        self.engine = create_engine("sqlite:///"+db_path)
        self.metadata = MetaData()
        self.metadata.create_all(self.engine)

    def AddKlient(self):
        Session = sessionmaker(bind=self.engine)
        session = Session()
        new_klient = Klient()
        session.add(new_klient)
        session.commit()