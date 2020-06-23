#include <cstdint>
#include <iostream>
#include <list>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "address.h"
#include "httpd.h"

#define ever (;;)

using webon::httpd;

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

int httpd::accept() const
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

int httpd::Start()
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

