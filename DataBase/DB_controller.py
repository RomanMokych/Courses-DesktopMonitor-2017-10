from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.types import DECIMAL

Base = declarative_base()

class Client(Base):
    __tablename__ = 'Client'
    id = Column(Integer, primary_key=True, autoincrement=True)
    ip = Column(String(45), unique=True)

    def __init__(self, ip):
        self.ip = ip

class Frame(Base):
    __tablename__ = 'Frame'
    id = Column(Integer, primary_key=True)
    time = Column(DECIMAL)
    client_id = Column(Integer, ForeignKey('Client.id'))
    parent = relationship("Client", backref="frame")

    def __init__(self, time):
        self.time = time

class FramePart(Base):
    __tablename__ = 'FramePart'
    id = Column(Integer, primary_key=True)
    time = Column(DECIMAL)
    lefttop_x = Column(Integer)
    lefttop_y = Column(Integer)
    frame_id = Column(Integer, ForeignKey('Frame.id'))
    parent = relationship("Frame", backref="FramePart")

    def __init__(self, time, frameId, x, y):
        self.time = time
        self.frame_id = frameId
        self.lefttop_x = x
        self.lefttop_y = y

class Role(Base):
    __tablename__ = 'Role'
    token = Column(String(45), primary_key=True, unique=True)
    user_name = Column(String(45))

    def __init__(self, role, token):
        self.token = token
        self.user_name = role
