#include <cstdint>
#include <iostream>
#include <list>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "address.h"
#include "httpd.h"
#include "worker.h"
#include "utility.h"

using webon::httpd;

namespace {
  template<typename Function, typename... Args>
  uint32_t invoke_socket_api(std::string_view activity_message, bool non_zero_ok, Function function, Args... args)
  {
    int result = function(args...);

    if (result < 0)
    {
      std::cout << "There was an error " << activity_message << ": " << errno << "." << std::endl;
      return result;
    }

    if (!non_zero_ok && (result != 0))
      std::cout << "Unknown result " << activity_message << ": " << result << std::endl;

    return result;
  }
}

int httpd::create_socket()
{
  return _socket = invoke_socket_api("creating socket", true, &::socket, PF_INET, SOCK_STREAM, 0);
}

int httpd::bind() const
{
  sockaddr_in address {0};
  address.sin_family = AF_INET;
  address.sin_port = htons(_port);
  address.sin_addr.s_addr = htonl(_address);

  return invoke_socket_api("binding socket", false, &::bind, _socket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}

int httpd::listen() const
{
  return invoke_socket_api("listening on socket", false, &::listen, _socket, 0);
}

int httpd::accept(IPv4& client_address, Port16& client_port) const
{
  sockaddr address{};
  socklen_t length = sizeof(address);
  int result = invoke_socket_api("accepting new connections", true, &::accept, _socket, &address, &length);

  if (result != -1)
  {
    auto const& client_in_address = reinterpret_cast<sockaddr_in&>(address);

    client_address = ntohl(client_in_address.sin_addr.s_addr);
    client_port = ntohs(client_in_address.sin_port);
  }

  return result;
}

int httpd::receive(int handle)
{
  std::cout << "Received new connection with file handle " << handle << "." << std::endl;

  _threads.push_back(std::thread(&thread_main, handle));

  return 0;
}

void httpd::thread_main(int handle)
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

int httpd::Close() const
{ 
  return invoke_socket_api("closing socket", false, &::close, _socket);
}

int httpd::Start()
{
  int r;

  if ((r = create_socket()) < 0) return r;
  if ((r = bind()) != 0) return r;
  if ((r = listen()) < 0) {
    Close();
    return r;
  }

  std::cout << "Now listening on port " << uint16_t(_port) << "." << std::endl;

  for ever
  {
    IPv4 client_address;
    Port16 client_port;

    if ((r = accept(client_address, client_port)) == -1) continue;

    Worker worker{_address, _port, client_address, client_port, r};
    _threads.push_back(std::thread(&Worker::Thread_Main, std::move(worker)));
  }
}

