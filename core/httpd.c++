#include <cstdint>
#include <iostream>
#include <list>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "address.h"
#include "console-helper.h"
#include "httpd.h"
#include "worker.h"
#include "utility.h"

using webon::httpd;

namespace {
  template<typename Function, typename... Args>
  uint32_t invoke_socket_api(std::string_view activity_message, bool non_zero_ok, Function function, Args... args)
  {
    int result = function(args...);

    if ((result < 0) && !activity_message.empty())
    {
      std::cout << "There was an error " << activity_message << ": " << errno << "." << std::endl;
      return result;
    }

    if (!non_zero_ok && (result != 0))
      std::cout << "Unknown result " << activity_message << ": " << result << std::endl;

    return result;
  }
}

httpd::httpd(Configuration&& configuration):
    _configuration{configuration}
{}

httpd::~httpd()
{
  Stop();
}

int httpd::create_socket()
{
  _socket = invoke_socket_api("creating socket", true, &::socket, PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

  if (_socket > -1)
    std::cout << "Created socket " << _socket << std::endl;
  else
    std::cout << "There was an error creating socket: " << errno << std::endl;

  return _socket;
}

int httpd::bind() const
{
  sockaddr_in address {0};
  address.sin_family = AF_INET;
  address.sin_port = htons(_configuration.port());
  address.sin_addr.s_addr = htonl(_configuration.ip_address());

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
  int result = invoke_socket_api("", true, &::accept, _socket, &address, &length);

  if (result != -1)
  {
    auto const& client_in_address = reinterpret_cast<sockaddr_in&>(address);

    client_address = ntohl(client_in_address.sin_addr.s_addr);
    client_port = ntohs(client_in_address.sin_port);
  }

  return result;
}

int httpd::Start()
{
  int r;

  if ((r = create_socket()) < 0) return r;
  if ((r = bind()) != 0) return r;
  if ((r = listen()) < 0) {
    Stop();
    return r;
  }

  std::cout << "Now listening on port " << uint16_t(_configuration.port()) << "." << std::endl;

  console::Initialize();
  int sleep_micros = 0;

  for ever
  {
    int ch = console::Check_For_Key_Press();

    if (ch != 0)
      break;

    IPv4 client_address;
    Port16 client_port;

    r = accept(client_address, client_port);

    if (r == -1)
    {
      int error = errno;

      if ((error == EWOULDBLOCK) || (error == EAGAIN))
      {
        usleep (sleep_micros);
        error = 0;

        if (sleep_micros < 10000)
          sleep_micros+= 1000;
      } else  {
        std::cout << "There was an error trying to receive new connection: " << strerror(errno) << " (" << errno << ")" << "." << std::endl;
      }

      continue;
    }

    Worker worker {
      _configuration.www_root(),
      _configuration.ip_address(),
      _configuration.port(),
      client_address,
      client_port,
      r
    };

    _threads.push_back(std::thread(&Worker::Thread_Main, std::move(worker)));
  }

  Stop();
  return 0;
}

int httpd::Stop()
{ 
  Worker::End_All();

  for (auto& thread: _threads)
    thread.join();

  _threads.clear();
  int result = 0;

  if (_socket != -1)
  {
    std::cout << "Closing socket " << _socket << "." << std::endl;
    result = invoke_socket_api("closing socket", false, &::close, _socket);
    _socket = -1;
  }

  console::Uninitialize();

  return result;
}

