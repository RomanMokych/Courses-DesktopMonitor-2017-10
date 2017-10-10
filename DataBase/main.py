from DataBase.DB_controller import  Klient,Frame,Context
conx = Context("D:\Db.db")



#conx.AddKlient("ip"+count)
#conx.AddFrame("ip1"+count,"path"+count,"time"+count)
conx.AddFrame("ip1","path5","time7")

res = conx.GetFramePathes("ip1")

mur = res.fetchall()
print(mur)

if len(mur):
    print("yes")