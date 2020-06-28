#include "request.h"

#include "utility.h"

using webon::Request;

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

webon::Request_Ptr Request::Create(string&& first_line)
{
  string method, protocol;

  if (!Parse_First_Line(first_line, method, protocol))
    return nullptr;

  if (method == "GET")
  {
    return Request_Ptr{new (std::nothrow) GET_Request(std::move(protocol))};
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

    _rep.back()+= line.substr(first_nonspace);
  } else {
    _rep.push_back(std::move(line));
  }
}

