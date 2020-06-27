#include <cstdint>
#include <iostream>
#include <list>
#include <thread>

#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

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

httpd::httpd(address::IPv4 address, address::Port16 port):
    _address{address},
    _port{port},
    _socket{-1},
    _threads{}
{}

httpd::httpd(address::IPv4 address):
    _address{address},
    _port{80},
    _socket{-1},
    _threads{}
{}

httpd::httpd(address::Port16 port):
    _address{},
    _port{port},
    _socket{-1},
    _threads{}
{}

httpd::httpd():
    _address{},
    _port{80},
    _socket{-1},
    _threads{}
{}

httpd::~httpd()
{
  Close();
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
  int result = invoke_socket_api("", true, &::accept, _socket, &address, &length);

  if (result != -1)
  {
    auto const& client_in_address = reinterpret_cast<sockaddr_in&>(address);

    client_address = ntohl(client_in_address.sin_addr.s_addr);
    client_port = ntohs(client_in_address.sin_port);
  }

  return result;
}

int httpd::Close()
{ 
  if (_socket == -1)
    return 0;

  std::cout << "Closing socket " << _socket << "." << std::endl;
  int result = invoke_socket_api("closing socket", false, &::close, _socket);

  if (result == 0)
    _socket = -1;
  else
    std::cout << "There was an error closing socket " << _socket << ": " << errno << std::endl;

  return result;
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

  termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);

  termios ttysave = ttystate;
  ttystate.c_lflag &= ~(ICANON | ECHO);
  ttystate.c_cc[VMIN] = 1;

  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

  int sleep_micros = 0;

  for ever
  {
    char ch = 0;

    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(fileno(stdin), &readset);

    timeval tv {
      .tv_sec = 1,
      .tv_usec = 0
    };

    select(fileno(stdin) + 1, &readset, nullptr, nullptr, &tv);

    if(FD_ISSET(fileno(stdin), &readset))
    {
      std::cout << "Key was pressed: " << getchar() << ".  Exiting." << std::endl;
      break;
    }

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

    Worker worker{_address, _port, client_address, client_port, r};
    _threads.push_back(std::thread(&Worker::Thread_Main, std::move(worker)));
  }

  ttystate.c_lflag |= ICANON | ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);

  Worker::End_All();

  for (auto& thread: _threads)
    thread.join();

  return 0;
}

