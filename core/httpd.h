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
      explicit httpd(address::IPv4 address, address::Port16 port);
      explicit httpd(address::IPv4 address);
      explicit httpd(address::Port16 port);
      explicit httpd();
      ~httpd();

      int Start();
      int Stop();
  };
}  // namespace webon

#endif  // WEBON__HTTPD_H__INCLUDED

