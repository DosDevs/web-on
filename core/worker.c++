#include "worker.h"

#include <iomanip>
#include <iostream>

#include <unistd.h>

#include "request.h"
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
  _handle{handle},
  _file{fdopen(_handle, "rw")}
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
  _handle{that._handle},
  _file{that._file}
{
  that._handle = -1;
  that._file = nullptr;
}

Worker::~Worker()
{
  if (!_file)
    return;

  std::cout << "[" << _handle << "] Worker has ended." << std::endl;

  fclose(_file);

  _file = nullptr;
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
  std::unique_ptr<Request> request;
  string line;
  bool cr = false;
  bool done = false;

  while (!done && _should_run)
  {
    int ch = getc(_file);

    switch (ch)
    {
      case EOF:
        return;

      case '\r':
        cr = true;
        break;

      case '\n':
        if (cr)
        {
          cr = false;

          if (!request)
          {
            request = Request::Create(std::move(line));
            line.clear();
          } else
          if (line.empty())
          {
            done = true;
          } else {
            request->Add(std::move(line));
            line.clear();
          }
        }

        break;

      default:
        if (cr)
          line.push_back('\r');

        cr = false;
        line.push_back(char(ch));

        break;
    }
  }

  std::cout << "[" << _handle << "] Request:" << *request << std::endl;
}

