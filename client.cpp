#include <iostream>

#include "gen-cpp/Example.h"

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TCompactProtocol.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

int main(int argc, char **argv) {
  boost::shared_ptr<TSocket>    socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
  boost::shared_ptr<TProtocol>  protocol(new TCompactProtocol(transport));

  ExampleClient client(protocol);

  transport->open();
  clock_t begin = clock();

  for (int i = 0; i < 1000; ++i) {
    client.ping();
  }
  clock_t end          = clock();
  double  elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC / 1000);
  cout << "Elapsed Time: " << elapsed_secs << "ms" << endl;
  transport->close();

  return 0;
}
