#include "request.h"

#include "utility.h"

using webon::Request;

webon::Request_Ptr Request::Create(string const& first_line)
{
  SHOW(first_line);
  if (String_Begins_With(first_line, "GET"))
  {
    return Request_Ptr{new Get_Request(first_line)};
  }

  return nullptr;
}
