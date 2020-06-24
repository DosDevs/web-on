#ifndef WEBON__WORKER_H__INCLUDED
#define WEBON__WORKER_H__INCLUDED

#include <stdio.h>

#include "address.h"


namespace webon
{
  using address::IPv4;
  using address::Port16;

  class Worker
  {
    private:
      IPv4 _server_address;
      Port16 _server_port;

      IPv4 _client_address;
      Port16 _client_port;

      int _handle;
      FILE* _file;

      void go() const;

    public:
      Worker(
          IPv4 server_address, Port16 server_port,
          IPv4 client_address, Port16 client_port,
          int handle);

      Worker(Worker&& that);

      ~Worker();

      static void Thread_Main(Worker&& worker);
  };
}  // namespace webon

#endif  // WEBON__WORKER_H__INCLUDED

