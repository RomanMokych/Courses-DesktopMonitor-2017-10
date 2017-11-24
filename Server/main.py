from Server.ServWrapper import DeskServer
from datetime import datetime

clientPass = b"1QqatNFb"
viewerPass = b"JjFaZqOT"
conStr = "mysql://vova:1234@127.0.0.1:3306/desktop"


server = DeskServer(conStr, "D:/pics", 8000)

#server.dbConx.AddRole('client', clientPass)
#server.dbConx.AddRole('viewer', viewerPass)
#server.dbConx.AddClient("ip1")


server.Run()

