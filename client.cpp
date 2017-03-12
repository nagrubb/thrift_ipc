#include <iostream>

#include "gen-cpp/Example.h"
#include "thrift_service.h"

int main(int argc, char **argv) {
  uint16_t port = 9090;

  boost::shared_ptr<thrift_client<ExampleClient> > client(new thrift_client<ExampleClient>(port));
  clock_t begin = clock();

  for (int i = 0; i < 1; ++i) {
    (*client)->ping();
  }
  clock_t end          = clock();
  double  elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC / 1000);
  std::cout << "Elapsed Time: " << elapsed_secs << "ms" << std::endl;

  return 0;
}
