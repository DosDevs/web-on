#ifndef WEBON__HTTPD_H__INCLUDED
#define WEBON__HTTPD_H__INCLUDED

#include <cstdint>
#include <string.h>
#include <errno.h>

#include "address.h"
#include "request.h"

namespace webon
{
  using address::IPv4;
  using address::Port16;

  class httpd
  {
    private:
      typedef uint32_t (*socket_api)();

      address::IPv4 _address;
      address::Port16 _port;
      int _socket;

      std::list<std::thread> _threads;

      int create_socket();
      int bind() const;
      int listen() const;
      int accept(IPv4& client_address, Port16& client_port) const;

    public:
      explicit httpd(address::IPv4 address, address::Port16 port):
          _address{address},
          _port{port},
          _socket{-1},
          _threads{}
      {}

      explicit httpd(address::IPv4 address):
          _address{address},
          _port{80},
          _socket{-1},
          _threads{}
      {}

      explicit httpd(address::Port16 port):
          _address{},
          _port{port},
          _socket{-1},
          _threads{}
      {}

      explicit httpd():
          _address{},
          _port{80},
          _socket{-1},
          _threads{}
      {}

      int Start();
      int Close() const;
  };
}  // namespace webon

#endif  // WEBON__HTTPD_H__INCLUDED

