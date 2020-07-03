#ifndef WEBON__REQUEST_H__INCLUDED
#define WEBON__REQUEST_H__INCLUDED

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "method.h"
#include "utility.h"

namespace webon
{
  using std::map;
  using std::string;
  using std::string_view;
  using std::vector;

  class Request;
  class Response;

  using Request_Ptr = std::unique_ptr<Request>;

  class Request
  {
    private:
      string const _protocol;
      map<string, string> _headers;
      vector<string> _content;

      static bool split(string_view line, char separator, string& first, string& second);

    public:
      static bool Parse_First_Line(string_view first_line, string& method, string& protocol);
      static bool Parse_Header(string_view line, string& header, string& value);

      static Request_Ptr Create(string&& first_line);

      Request(string&& protocol):
        _protocol{std::move(protocol)},
        _headers{},
        _content{}
      {}

      virtual ~Request() {}

      virtual Method Get_Method() const = 0;
      virtual Response Execute() const = 0;

      string const& Get_Protocol() const
      {
        return _protocol;
      }

      map<string, string> const& Get_Headers() const
      {
        return _headers;
      }

      vector<string> const& Get_Lines() const
      {
        return _content;
      }

      void Add(string&& line);
  };

  template<typename Stream>
  inline Stream& operator<<(Stream& stream, Request const& request)
  {
    stream << '\n' << Get_Method_Name(request.Get_Method()) << " / " << request.Get_Protocol() << '\n';

    for (auto const& item: request.Get_Headers())
      stream << item.first << ": " << item.second << '\n';

    for (auto const& line: request.Get_Lines())
      stream << line << '\n';

    return stream;
  }
}  // namespace webon

#endif  // WEBON__REQUEST_H__INCLUDED

