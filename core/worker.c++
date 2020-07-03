#include "worker.h"

#include <iomanip>
#include <iostream>

#include <unistd.h>

#include "request.h"
#include "response.h"
#include "utility.h"

using webon::Worker;

bool Worker::_should_run = true;

Worker::Worker(
    IPv4 server_address,
    Port16 server_port,
    IPv4 client_address,
    Port16 client_port,
    int handle):
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
  worker.go();
}

void Worker::End_All()
{
  _should_run = false;
}

void Worker::go() const
{
  bool done = false;

  while (!done)
  {
    std::unique_ptr<Request> request{};
    string line{};

    for ever
    {
      char ch = 0;
      int result = read(_handle, &ch, 1);

      if (!_should_run || (result == 0))
      {
        std::cout << "Client finished sending data." << std::endl;
        done = true;
        break;
      }

      if ((result == -1) && (errno == EAGAIN))
      {
        usleep(10000);
      }

      if (ch == 0)
      {
        done = false;
        break;
      }

      line.push_back(ch);

      if (String_Ends_With(line, CRLF))
      {
        line.pop_back();
        line.pop_back();

        if (!request)
          request = Request::Create(std::move(line));
        else
        if (!line.empty())
          request->Add(std::move(line));

        line.clear();
      }
    }

    if (!_should_run)
      break;

    if (request)
    {
      std::cout << "[" << _handle << "] Request:" << *request << std::endl;

      auto const response = request->Execute();
      std::cout << "[" << _handle << "] Response:" << response << std::endl;
    }
  }

  std::cout << "[" << _handle << "] Worker is shutting down." << std::endl;
}

