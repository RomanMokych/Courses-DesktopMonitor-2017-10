from DataBase.DB_controller import  Client,Context
from sqlalchemy.dialects.mysql import DATETIME

conx = Context("mysql://vova:1234@127.0.0.1:3306/desktopdb")


ff= conx.GetRangeFrames("2020-10-17 17:00:00","2010-10-17 17:00:00",1)
f = ff.fetchall()
print(f)
for row in ff:
    print(row)
