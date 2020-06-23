#include <iostream>
#include <list>
#include <string_view>
#include <thread>

#include <stdio.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ever (;;)

namespace webon
{
  namespace address
  {
    inline constexpr uint32_t makeui32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
      return (uint32_t(a) << 24) | (b << 16) | (c << 8) | d;
    }

    class IPv4
    {
      private:
        uint32_t _rep;

      public:
        constexpr IPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d): _rep(makeui32(a, b, c, d)) {}

        constexpr IPv4(uint32_t value): _rep(value) {}

        constexpr IPv4(): IPv4(0) {}

        constexpr operator uint32_t() const { return _rep; }

        operator std::string() const {
          int big_endian = htonl(_rep);

          return
              std::move(
                  std::to_string(big_endian & 0xff) + std::string(".") +
                  std::to_string((big_endian >> 8) & 0xff) + std::string(".") +
                  std::to_string((big_endian >> 16) & 0xff) + std::string(".") +
                  std::to_string((big_endian >> 24) & 0xff));
        }
    };

    class Port16
    {
      private:
        uint16_t _rep;

      public:
        constexpr Port16(uint16_t value): _rep(value) {}

        constexpr Port16(): Port16(0) {}

        constexpr operator uint16_t() const { return _rep; }

        operator std::string() const {
          return std::move(std::to_string(_rep));
        }
    };
  }

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
          std::cout << "There was an error " << activity_message << ": " << errno << "." << std::endl;
          return 1;
        }

        if (!non_zero_ok && (result != 0))
          std::cout << "Unknown result " << activity_message << ": " << result << std::endl;

        return result;
      }

      int create_socket()
      {
        return _socket = invoke_socket_api("creating socket", true, &::socket, PF_INET, SOCK_STREAM, 0);
      }

      int bind() const
      {
        sockaddr_in address {0};
        address.sin_family = AF_INET;
        address.sin_port = htons(_port);
        address.sin_addr.s_addr = htonl(_address);

        return invoke_socket_api("binding socket", false, &::bind, _socket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
      }

      int listen() const
      {
        return invoke_socket_api("listening on socket", false, &::listen, _socket, 0);
      }

      int accept() const
      {
        sockaddr client_address;
        socklen_t address_length;
        int result = invoke_socket_api("accepting new connections", true, &::accept, _socket, &client_address, &address_length);

        if (result != -1)
        {
          auto const& client_in_address = reinterpret_cast<sockaddr_in&>(client_address);
          address::IPv4 client_ipv4_address = ntohl(client_in_address.sin_addr.s_addr);
          address::Port16 port = ntohs(client_in_address.sin_port);

          std::cout << "Received a new connection from " << std::string(client_ipv4_address) << ":" << port << "." << std::endl;
        }

        return result;
      }

      static void thread_main(int handle)
      {
        FILE* file = fdopen(handle, "rw");
        std::string line;

        for ever
        {
          switch (int ch = getc(file); ch)
          {
            case EOF:
              return;

            case '\n':
              std::cout << "[" << handle << "] " << line << std::endl;
              line.clear();
              break;

            default:
              line.push_back(char(ch));
              break;
          }
        }
      }

      int receive(int handle)
      {
        std::cout << "Received new connection with file handle " << handle << "." << std::endl;

        _threads.push_back(std::thread(&thread_main, handle));

        return 0;
      }

    public:
      httpd(address::IPv4 address, address::Port16 port):
          _address(address),
          _port(port),
          _socket(-1),
          _threads()
      {}

      int Listen()
      {
        int r;

        if ((r = create_socket()) == -1) return r;
        if ((r = bind()) == -1) return r;
        if ((r = listen()) == -1) return r;

        std::cout << "Now listening on port " << uint16_t(_port) << "." << std::endl;

        for ever
        {
          if ((r = accept()) == -1) continue;
          
          receive(r);
        }
      }
  };
}  // namespace webon

int main()
{
  webon::httpd httpd{webon::address::IPv4{}, webon::address::Port16{80}};

  if (int r = httpd.Listen(); r != 0)
  {
    std::cout << "There was an error starting web server: " << r << "." << std::endl;
    return r;
  }

  return 0;
}

