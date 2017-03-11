#!/usr/bin/env python
import sys
import glob
import time
import threading

sys.path.append('build/gen-py')

from example import Example, EventHandler

from thrift import Thrift
from thrift.server import TServer
from thrift.transport import TSocket, TTransport
from thrift.protocol import TCompactProtocol

PORT=9089

class EventHandlerHandler:
  def __init__(self):
    self.log = {}

  def event(self, evt):
    print "event({})".format(evt)

def server():
  handler = EventHandlerHandler()
  processor = EventHandler.Processor(handler)
  transport = TSocket.TServerSocket(port=PORT)
  tfactory = TTransport.TFramedTransportFactory()
  pfactory = TCompactProtocol.TCompactProtocolFactory()

  server = TServer.TSimpleServer(processor, transport, tfactory, pfactory)

  print "Starting python server..."
  server.serve()

def main():
  t = threading.Thread(target=server)
  t.start()

  transport = TSocket.TSocket('localhost', 9090)
  transport = TTransport.TFramedTransport(transport)
  protocol = TCompactProtocol.TCompactProtocol(transport)
  client = Example.Client(protocol)

  transport.open()

  print 'register'
  client.registerHandler(PORT)
  print 'done register'


  start = time.time()
  for i in range(0, 4):
    client.wait()
  end = time.time()
  elapsed = end - start
  print 'took', elapsed * 1000, 'ms'
  transport.close()

if __name__ == '__main__':
  main()
