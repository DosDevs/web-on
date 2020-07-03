#include "request.h"

#include <string>
#include <string_view>

#include <unistd.h>

#include "response.h"
#include "url.h"
#include "utility.h"

namespace webon
{
  class OPTIONS_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::OPTIONS; }
  };

  class GET_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::GET; }
      Response Execute() const override;

      string Get_Resource_File_Name(address::URL const& url) const;
  };

  class HEAD_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::HEAD; }
  };

  class POST_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::POST; }
  };

  class PUT_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::PUT; }
  };

  class DELETE_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::DELETE; }
  };

  class TRACE_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::TRACE; }
  };

  class CONNECT_Request: public Request
  {
    public:
      using Request::Request;

      Method Get_Method() const override { return Method::CONNECT; }
  };

  bool Request::split(string_view line, char separator, string& first, string& second)
  {
    auto const separator_offset = line.find(separator);

    if (separator_offset == string::npos)
      return false;

    first = std::move(trim(line.substr(0, separator_offset)));
    second = std::move(trim(line.substr(separator_offset + 1)));

    return !first.empty();
  }

  bool Request::Parse_First_Line(string_view first_line, string& method, string& protocol)
  {
    return (split(first_line, '/', method, protocol) && !protocol.empty());
  }

  bool Request::Parse_Header(string_view line, string& header, string& value)
  {
    return split(line, ':', header, value);
  }

  Request_Ptr Request::Create(string const& www_root, string&& first_line)
  {
    string method, protocol;

    if (!Parse_First_Line(first_line, method, protocol))
      return nullptr;

    if (method == "GET")
    {
      return Request_Ptr{new (std::nothrow) GET_Request(www_root, std::move(protocol))};
    }

    return nullptr;
  }

  void Request::Add(string&& line)
  {
    string header, value;
    if (Parse_Header(line, header, value)) {
      _headers[header] = value;
    } else
    if (!line.empty() && (is_space(line[0]) || is_ht(line[0])))
    {
      auto const first_nonspace = line.find_first_not_of(SPACES);

      if (first_nonspace == string::npos)
        return;

      _content.back()+= line.substr(first_nonspace);
    } else {
      _content.push_back(std::move(line));
    }
  }

  Response GET_Request::Execute() const
  {
    Response response{Get_Method(), Get_Protocol(), *this};

    // Assume GET is only for /.
    auto url { address::URL::Parse("/") };

    string const file_name = Get_Resource_File_Name(url);
    string const file_contents = Read_File(file_name);

    response.Set_Status(file_contents.empty()? "404" : "200 OK");
    response.Add(file_contents);

    return response;
  }

  string GET_Request::Get_Resource_File_Name(address::URL const& url) const
  {
    string file_name_base = Get_WWW_Root() +
          ((url.Get_Resource() == "/")? "/index": url.Get_Resource());

    string file_name = file_name_base + ".html";
    if (File_Is_Accesible(file_name))
      return file_name;

    file_name = file_name_base + ".htm";
    if (File_Is_Accesible(file_name))
      return file_name;

    file_name.clear();
    return file_name;
  }
}  // namespace webon

