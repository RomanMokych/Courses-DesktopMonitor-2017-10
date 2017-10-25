from twisted.internet import reactor, protocol

class ServHandler(protocol.Protocol):
    def dataReceived(self, data):
        self.data = data;
        print(data)

    def connectionMade(self):
        print("connection")
        print("connection from", self.transport.getHost().host)

    def connectionLost(self, reason):
        print("connection lost",reason)

class DeskServer():
    def __init__(self):
        self.factory = protocol.ServerFactory()
        self.factory.protocol= ServHandler
        reactor.listenTCP(8000,self.factory)

    def run(self):
        reactor.run()


ff = DeskServer()
ff.run()

