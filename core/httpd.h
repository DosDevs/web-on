#ifndef WEBON__HTTPD_H__INCLUDED
#define WEBON__HTTPD_H__INCLUDED

#include <cstdint>
#include <string.h>
#include <errno.h>

#include "address.h"

namespace webon
{
  class httpd
  {
    private:
      typedef uint32_t (*socket_api)();

      address::IPv4 _address;
      address::Port16 _port;
      int _socket;

      std::list<std::thread> _threads;

      template<typename Function, typename... Args>
      uint32_t invoke_socket_api(std::string_view activity_message, bool non_zero_ok, Function function, Args... args) const
      {
        int result = function(args...);

        if (result == -1)
        {
          std::cout << "There was an error " << activity_message << ": " << strerror(errno) << " (" << errno << ")" << "." << std::endl;
          return 1;
        }

        if (!non_zero_ok && (result != 0))
          std::cout << "Unknown result " << activity_message << ": " << result << std::endl;

        return result;
      }

      int create_socket();
      int bind() const;
      int listen() const;
      int accept() const;
      int receive(int handle);

      static void thread_main(int handle);

    public:
      httpd(address::IPv4 address, address::Port16 port):
          _address(address),
          _port(port),
          _socket(-1),
          _threads()
      {}

      int Start();
      int Close() const;
  };
}  // namespace webon

#endif  // WEBON__HTTPD_H__INCLUDED

