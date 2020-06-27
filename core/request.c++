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
    _rep.push_back(std::move(line));
  }
}

