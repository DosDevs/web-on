#include "worker.h"

#include <iomanip>
#include <iostream>

#include <sys/socket.h>
#include <unistd.h>

#include "request.h"
#include "response.h"
#include "utility.h"

using webon::Worker;

bool Worker::_should_run = true;

Worker::Worker(
    string const& www_root,
    IPv4 server_address,
    Port16 server_port,
    IPv4 client_address,
    Port16 client_port,
    int handle):
  _www_root{www_root},
  _server_address{server_address},
  _server_port{server_port},
  _client_address{client_address},
  _client_port{client_port},
  _handle{handle}
{
  std::cout
        << "Received a new connection from " << _client_address << ":" << _client_port
        << " with file handle " << _handle << "." << std::endl;
}

Worker::Worker(Worker&& that):
  _www_root{std::move(that._www_root)},
  _server_address{std::move(that._server_address)},
  _server_port{std::move(that._server_port)},
  _client_address{std::move(that._client_address)},
  _client_port{std::move(that._client_port)},
  _handle{that._handle}
{
  that._handle = -1;
}

Worker::~Worker()
{
  if (_handle == -1)
    return;

  std::cout << "[" << _handle << "] Worker has ended; closing socket." << std::endl;
  close(_handle);

  _handle = -1;
}

void Worker::Thread_Main(Worker&& worker)
{
  worker._go();
}

void Worker::End_All()
{
  _should_run = false;
}

void Worker::_go() const
{
  std::unique_ptr<Request> request{};
  string line{};

  for ever
  {
    string line;
    int result = _read_line(line);

    if (!_should_run || (result == 0))
    {
      std::cout << "Client finished sending data." << std::endl;
      break;
    }

    if (!request)
      request = Request::Create(_www_root, std::move(line));
    else
    if (!line.empty())
      request->Add(std::move(line));
  }

  if (!_should_run)
    return;

  if (request)
  {
    std::cout << "[" << _handle << "] Request:" << *request << std::endl;

    auto const response = request->Execute();
    std::cout << "[" << _handle << "] Response:" << response << std::endl;

    string const headline = response.Get_Protocol() + " " + response.Get_Status();

    if (_write_line(headline) != 0)
      return;

    if (_write(CRLF) != 0)
      return;

    for (auto const& line: response.Get_Lines())
      if (_write_line(line) != 0)
        break;
  }

  std::cout << "[" << _handle << "] Worker is shutting down." << std::endl;
}

int Worker::_read_line(string& line) const
{
  line.clear();

  for ever
  {
    char ch = 0;
    int result = read(_handle, &ch, 1);

    if (!_should_run || (result == 0))
      return 0;

    if ((result == -1) && (errno == EAGAIN))
    {
      usleep(10000);
      continue;
    }

    if (ch == 0)
      return 0;

    line.push_back(ch);

    if (String_Ends_With(line, CRLF))
    {
      line.pop_back();
      line.pop_back();
      return line.length() + 2;
    }
  }
}

int Worker::_write(string_view text) const
{
  int result = 0;
  int offset = 0;

  do {
    result = write(_handle, &text[0], text.length());

    if (result > 0)
      offset+= result;
  } while ((offset < text.length()) && (errno == EAGAIN));

  if (result < 0)
  {
    std::cout << "Error writing response to socket: " << strerror(errno) << " (" << errno << ")" << "." << std::endl;
    return errno;
  }

  return 0;
}

int Worker::_write_line(string const& line) const
{
  int result = 0;

  if ((result = _write(line)) != 0)
    return result;

  if ((result = _write(CRLF)) != 0)
    return result;

  return 0;
}

