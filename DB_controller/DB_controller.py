from sqlalchemy import create_engine

class DB_controller:
    connector= create_engine("sqlite://frame.db")










#from sqlalchemy.ext.declarative import declarative_base
#Base = declarative_base()
#from sqlalchemy import Column, Integer, String

#Base = declarative_base()
#class User(Base):
 #   __tablename__= "User"
  #  id = Column(Integer,primary_key="true")
   # name = Column(String)
    #fullname = Column(String)

    #def __repr__(self):
     #   return "<User(%r, %r)>" % (
      #  self.name, self.fullname
   #     )
#




