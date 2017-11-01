from twisted.internet import reactor, protocol
from DataBase.DB_controller import Context

class ServHandler(protocol.Protocol):
    buff = list()
    def dataReceived(self, data):
        #парсим дату,картинку
        self.buff+=str(data)
        print("received")

    def connectionMade(self):
        print("connection from", self.transport.getHost().host)
        #Если клиент неопознан добавляем
        #Если клиент опознан ничего не делаем

    def connectionLost(self, reason):
        print("connection lost",reason)

class DeskServer():
    def __init__(self,dbConStr,mainPath,port):
        self.factory = protocol.ServerFactory()
        self.factory.protocol= ServHandler
        self.factory.protocol.parent= self
        reactor.listenTCP(port,self.factory)
        self.dbConx = Context(dbConStr)
        self.path = mainPath

    def run(self):
        reactor.run()
    #############################################################
    def savePic(self,data,client_ip):
        client = self.dbConx.GetClient(client_ip)

        #Если директории клиент нет создаем
        #добавяем фрейм
        #сохраняем в директорию
    ################################################################







