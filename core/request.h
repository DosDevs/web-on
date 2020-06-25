#ifndef WEBON__REQUEST_H__INCLUDED
#define WEBON__REQUEST_H__INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include "utility.h"

namespace webon
{
  using std::string;
  using std::string_view;
  using std::vector;

  class Request;
  using Request_Ptr = std::unique_ptr<Request>;

  class Request
  {
    private:
      vector<string> _rep;

    public:
      static Request_Ptr Create(string const& first_line);

      Request(string const& first_line):
        _rep(1, first_line)
      {}

      virtual ~Request() {}

      constexpr void Add(string&& line)
      {
        _rep.push_back(std::move(line));
      }

      constexpr vector<string> const& Lines() const
      {
        return _rep;
      }
  };

  template<typename Stream>
  inline constexpr Stream& operator<<(Stream& stream, Request const& request)
  {
    std::cout << std::endl;

    for (auto const& line: request.Lines())
      std::cout << line << std::endl;

    return stream;
  }

  class Get_Request: public Request
  {
    public:
      using Request::Request;
  };
}  // namespace webon

#endif  // WEBON__REQUEST_H__INCLUDED

