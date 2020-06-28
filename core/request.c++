#include "request.h"

#include "utility.h"

using webon::Request;

bool Request::Parse_First_Line(string_view first_line, string& method, string& protocol)
{
  auto const slash_offset = first_line.find('/');

  if (slash_offset == string::npos)
    return false;

  method = trim(first_line.substr(0, slash_offset));
  protocol = trim(first_line.substr(slash_offset + 1));

  return (!method.empty() && !protocol.empty());
}

bool Request::Parse_Header(string_view line, string& header, string& value)
{
  auto const colon_offset = line.find(':');
  if (colon_offset == string::npos)
    return false;
  
  header = trim(line.substr(0, colon_offset));
  value = trim(line.substr(colon_offset + 1));

  return (!header.empty() && !value.empty());
}

webon::Request_Ptr Request::Create(string&& first_line)
{
  string method, protocol;

  if (!Parse_First_Line(first_line, method, protocol))
    return nullptr;

  if (method == "GET")
  {
    return Request_Ptr{new GET_Request(std::move(protocol))};
  }

  return nullptr;
}

void Request::Add(string&& line)
{
  if (!line.empty() && (is_space(line[0]) || is_ht(line[0])))
  {
    _rep.back()+= line;
  } else {
    string header, value;
    if (Parse_Header(line, header, value)) {
      _headers[header] = value;
    }
    _rep.push_back(std::move(line));
  }
}

