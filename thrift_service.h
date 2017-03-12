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
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", eventPort));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol>  protocol(new TCompactProtocol(transport));
    boost::shared_ptr<T> client(new T(protocol));
    transport->open();
    m_clients.push_back(client);
    return true;
  }

  virtual void notify_clients(std::function<void(T&)> handler) {
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
  std::list<boost::shared_ptr<T> > m_clients;
};

class thrift_application {
public:
  thrift_application(uint16_t io_thread_count) :
    m_thread_manager(ThreadManager::newSimpleThreadManager(io_thread_count)),
    m_thread_factory(new PosixThreadFactory()) {
    m_thread_manager->threadFactory(m_thread_factory);
    m_thread_manager->start();
  }

  ~thrift_application(void) {}

  bool add_and_start_runnable(boost::shared_ptr<Runnable> &runnable) {
    boost::shared_ptr<Thread> thread(m_thread_factory->newThread(runnable));
    m_threads.push_back(thread);
    thread->start();
    return true;
  }

  boost::shared_ptr<ThreadManager>& get_thread_manager() {
    return m_thread_manager;
  }

private:
  boost::shared_ptr<ThreadManager> m_thread_manager;
  boost::shared_ptr<PosixThreadFactory> m_thread_factory;
  std::list<boost::shared_ptr<Thread> > m_threads;
};

template<class T1, class T2>
class thrift_server : public Runnable {
public:
  thrift_server(boost::shared_ptr<thrift_application> &application, uint16_t port, boost::shared_ptr<T1> &handler) :
    m_port(port),
    m_handler(handler),
    m_processor(new T2(m_handler)),
    m_transport_factory(new TBufferedTransportFactory()),
    m_protocol_factory(new TCompactProtocolFactory()),
    m_application(application) {}

  virtual ~thrift_server(void) {}

  virtual void run() {
    boost::shared_ptr<TNonblockingServer> server(
      new TNonblockingServer(m_processor, m_protocol_factory, m_port, m_application->get_thread_manager()));
    server->serve();
  }

private:
  uint16_t m_port;
  boost::shared_ptr<T1> &m_handler;
  boost::shared_ptr<TProcessor> m_processor;
  boost::shared_ptr<TTransportFactory> m_transport_factory;
  boost::shared_ptr<TProtocolFactory> m_protocol_factory;
  boost::shared_ptr<thrift_application> &m_application;
};

template<class T>
class thrift_client {
public:
  thrift_client(uint16_t port) :
    m_socket(new TSocket("localhost", port)),
    m_transport(new TFramedTransport(m_socket)),
    m_protocol(new TCompactProtocol(m_transport)),
    m_client(new T(m_protocol)) {
    m_transport->open();
  }

  virtual ~thrift_client(void) {
    m_transport->close();
  }

  T& operator*() {
    return *m_client.get();
  }

  T *operator->() {
    return m_client.get();
  }

private:
  boost::shared_ptr<TSocket> m_socket;
  boost::shared_ptr<TTransport> m_transport;
  boost::shared_ptr<TProtocol> m_protocol;
  boost::shared_ptr<T> m_client;
};
