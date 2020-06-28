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
  bool done = false;
  while (!done)
  {
    std::unique_ptr<Request> request;
    string line;

    for ever
    {
      int ch = getc(_file);

      if (!_should_run || ch == EOF)
      {
        done = true;
        break;
      }

      line.push_back(ch);

      size_t length = line.length();
      if ((length > 1) && (line[length - 2] == '\r') && (line[length - 1] == '\n'))
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

    if (!request)
    {
      std::cout << "[" << _handle << "] No request." << std::endl;
      return;
    }

    std::cout << "[" << _handle << "] Request:" << *request << std::endl;
  }
}

