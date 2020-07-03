#ifndef WEBON__RESPONSE_H__INCLUDED
#define WEBON__RESPONSE_H__INCLUDED

#include <map>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "method.h"
#include "utility.h"

namespace webon
{
  using std::map;
  using std::string;
  using std::vector;

  class Request;
  class Response;

  class Response
  {
    private:
      Method const _method;
      string const _protocol;
      Request const& _request;
      string _status;
      map<string, string> _headers;
      vector<string> _content;

    public:
      Response(Response&& that):
        _method{that._method},
        _protocol{that._protocol},
        _request{that._request},
        _status{std::move(that._status)},
        _headers{std::move(that._headers)},
        _content{std::move(that._content)}
      {}

      Response(Method method, string const& protocol, Request const& request):
        _method{method},
        _protocol{protocol},
        _request{request},
        _status{"404"},
        _headers{},
        _content{}
      {}

      Method Get_Method() const { return _method; };
      string const& Get_Protocol() const { return _protocol; };
      Request const& Get_Request() const { return _request; };
      string const& Get_Status() const { return _status; };
      map<string, string> const& Get_Headers() const { return _headers; };
      vector<string> const& Get_Lines() const { return _content; };

      void Set_Status(string const& status)
      {
        _status = status;
      }

      void Add(string const& line)
      {
        _content.push_back(line);
      }
  };

  template<typename Stream>
  inline Stream& operator<<(Stream& stream, Response const& response)
  {
    stream << '\n' << Get_Method_Name(response.Get_Method()) << " / " << response.Get_Protocol() << '\n';

    for (auto const& item: response.Get_Headers())
      stream << item.first << ": " << item.second << '\n';

    for (auto const& line: response.Get_Lines())
      stream << line << '\n';

    return stream;
  }
}  // namespace webon

#endif  // WEBON__RESPONSE_H__INCLUDED

