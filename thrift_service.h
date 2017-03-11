#pragma once

#include <thrift/Thrift.h>
#include <thrift/transport/TSocket.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/PosixThreadFactory.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::concurrency;

template<class T>
class thrift_registable_server {
public:

  thrift_registable_server(void) {}

  virtual ~thrift_registable_server(void) {}

  virtual bool register_client(const int16_t eventPort) {
    boost::shared_ptr<TSocket>    socket(new TSocket("localhost", eventPort));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol>  protocol(new TCompactProtocol(transport));
    boost::shared_ptr<T> client(new T(protocol));
    transport->open();
    m_clients.push_back(client);
    return true;
  }

  virtual void notify_clients(std::function<void(T&)>handler) {
    for (auto iter = m_clients.begin(); iter != m_clients.end(); ++iter) {
      try {
        handler(**iter);
      } catch (TException &thrift_exception) {
        // TODO: Notify client has been disconnected
        iter = m_clients.erase(iter);
      }
    }
  }

private:

  std::list<boost::shared_ptr<T> >m_clients;
};

template<class T1, class T2>
class thrift_server : public Runnable {
public:

  thrift_server(uint16_t port, uint16_t io_thread_count, T1 *iface) :
    m_port(port),
    m_handler(iface),
    m_processor(new T2(m_handler)),
    m_transport_factory(new TBufferedTransportFactory()),
    m_protocol_factory(new TCompactProtocolFactory()),
    m_thread_manager(ThreadManager::newSimpleThreadManager(io_thread_count)),
    m_thread_factory(new PosixThreadFactory()) {
    m_thread_manager->threadFactory(m_thread_factory);
    m_thread_manager->start();
  }

  virtual ~thrift_server(void) {}

  bool add_and_start_worker_thread(Runnable *runnable) {
    boost::shared_ptr<Runnable> wrapper(runnable);
    boost::shared_ptr<Thread>   thread(m_thread_factory->newThread(wrapper));
    m_threads.push_back(thread);
    thread->start();
    return true;
  }

  virtual void run() {
    TNonblockingServer server(m_processor, m_protocol_factory, m_port, m_thread_manager);

    server.serve();
  }

private:

  uint16_t m_port;
  boost::shared_ptr<T1>m_handler;
  boost::shared_ptr<TProcessor>m_processor;
  boost::shared_ptr<TTransportFactory>m_transport_factory;
  boost::shared_ptr<TProtocolFactory>m_protocol_factory;
  boost::shared_ptr<ThreadManager>m_thread_manager;
  boost::shared_ptr<PosixThreadFactory>m_thread_factory;
  std::list<boost::shared_ptr<Thread> >m_threads;
};
