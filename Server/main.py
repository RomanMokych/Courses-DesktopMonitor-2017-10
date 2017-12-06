from Server.ServWrapper import DeskServer
from Server.config import ServerConf

server = DeskServer(ServerConf.CONSTR, ServerConf.MAINDIR, ServerConf.PORT)
server.Run()

