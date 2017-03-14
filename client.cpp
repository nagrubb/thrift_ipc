#include <iostream>

#include <boost/make_shared.hpp>

#include "gen-cpp/Example.h"
#include "gen-cpp/EventHandler.h"
#include "thrift_service.h"


class CustomAsyncClient : public thrift_async_client<ExampleClient, EventHandlerIf, EventHandlerProcessor>{
public:
  CustomAsyncClient(boost::shared_ptr<thrift_application> &app, uint16_t server_port,
                    uint16_t client_port) : thrift_async_client<ExampleClient, EventHandlerIf,
                                                                EventHandlerProcessor>(app, server_port, client_port) {}

  virtual ~CustomAsyncClient(void) {}

  virtual void event(const int32_t sequence) {
    std::cout << "event: {" << sequence << "}" << std::endl;
  }
};

int main(int argc, char **argv) {
  uint16_t server_port     = 9089;
  uint16_t client_port     = 9090;
  uint16_t io_thread_count = 1;

  boost::shared_ptr<thrift_application> application(new thrift_application(io_thread_count));
  boost::shared_ptr<CustomAsyncClient>  client(new CustomAsyncClient(application, server_port, client_port));

  application->add_and_start_runnable(boost::dynamic_pointer_cast<Runnable>(client));

  clock_t begin = clock();

  for (int i = 0; i < 1; ++i) {
    (*client)->ping();
  }
  clock_t end          = clock();
  double  elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC / 1000);
  std::cout << "Elapsed Time: " << elapsed_secs << "ms" << std::endl;

  (*client)->registerHandler(server_port);
  (*client)->wait();

  while (true) {
    sleep(10);
  }

  return 0;
}
