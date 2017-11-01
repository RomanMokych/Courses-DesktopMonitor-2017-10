from DataBase.DB_controller import  Client,Context
from sqlalchemy.dialects.mysql import DATETIME
from Server.DataCrypter import Crypter
from Server.ServWrapper import  DeskServer


crptr = Crypter(b"MyHardKe")

val = b"111"
val_encr = crptr.encrypt(val)

print(val_encr)
val = crptr.decrypt(val_encr)
print(val)


#conStr= "mysql://vova:1234@127.0.0.1:3306/desktopdb"

#server  = DeskServer(conStr,".",8000)
#server.run()

