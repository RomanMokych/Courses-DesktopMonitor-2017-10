from __future__ import print_function
from twisted.internet import reactor, protocol


class EchoClient(protocol.Protocol):
    def connectionMade(self):
        self.transport.write(b"hello, world!")
        self.transport.loseConnection()

    def connectionLost(self, reason):
        print("connection lost")

class EchoFactory(protocol.ClientFactory):
    protocol = EchoClient

    def clientConnectionFailed(self, connector, reason):
        print("Connection failed - goodbye!")
        reactor.stop()

    def clientConnectionLost(self, connector, reason):
        print("Connection lost - goodbye!")
        reactor.stop()

f = EchoFactory()
reactor.connectTCP("localhost", 8000, f)
reactor.run()