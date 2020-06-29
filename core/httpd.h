#ifndef WEBON__HTTPD_H__INCLUDED
#define WEBON__HTTPD_H__INCLUDED

#include <cstdint>
#include <string.h>
#include <errno.h>

#include "address.h"
#include "configuration.h"
#include "request.h"

namespace webon
{
  using address::IPv4;
  using address::Port16;

  class httpd
  {
    private:
      typedef uint32_t (*socket_api)();

      Configuration _configuration;
      int _socket;

      std::list<std::thread> _threads;

      int create_socket();
      int bind() const;
      int listen() const;
      int accept(IPv4& client_address, Port16& client_port) const;

    public:
      explicit httpd(Configuration&& configuration);
      ~httpd();

      int Start();
      int Stop();
  };
}  // namespace webon

#endif  // WEBON__HTTPD_H__INCLUDED

