from DATA_BASE.DB_controller import Klient,Frame,Context

conx = Context("D:\Db.db")


#conx.AddKlient("ip2")
#conx.AddFrame("ip2","path2","time2")

res = conx.GetFrames("ip2")
print(res)