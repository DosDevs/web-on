#include "url.h"

#include <cstdint>
#include <map>
#include <regex>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

using webon::address::URL;

bool URL::_find(string const& what, string const& where, size_t& start, size_t& end, string& output)
{
  end = where.find(what, start);

  if (end == string::npos)
    return false;

  output.assign(&where[start], &where[end]);
  start = end;

  return true;
}

void URL::_extract_namespace(string source, size_t& start, size_t& end)
{
  if (_find("://", source, start, end, _namespace))
  {
    start+= 3;
  } else {
    _namespace = "http";
  }
}

bool URL::_extract_host_and_port(string source, size_t& start, size_t& end)
{
  if (!_find("/", source, start, end, _host))
  {
    _host = source.substr(start);
    start = source.length();
  }

  if (int pos = _host.find(':'); pos != string::npos)
  {
    _port = _host.substr(pos + 1);
    _host = _host.substr(0, pos);
  }

  static std::regex host_regex{R"(^(\w[\w\-]*\w\.?)*$)"};
  static std::regex port_regex{R"(^\w+$)"};

  return (
      std::regex_match(_host, host_regex) &&
      (_port.empty() || std::regex_match(_port, port_regex)));
}

void URL::_extract_resource(string source, size_t& start, size_t& end)
{
  if (_find("?", source, start, end, _resource))
  {
    ++start;
  } else {
    _resource = "/";
    start = source.length();
  }
}

bool URL::_extract_kvps(string source, size_t& start, size_t& end)
{
  while (start < source.length())
  {
    end = source.find_first_of("&#", start);
    size_t equals = source.find("=", start);

    if (equals > end)
      return false;

    string const key = source.substr(start, equals - start);
    ++equals;

    string const value = source.substr(equals, end - equals);

    if (end != string::npos)
      ++end;

    _kvps.emplace(key, value);
    start = end;
  }

  return true;
}

URL URL::Parse(string const& source)
{
  URL url{};

  size_t start = 0;
  size_t end = 0;

  url._extract_namespace(source, start, end);

  if (!url._extract_host_and_port(source, start, end))
  {
    url.Clear();
    return url;
  }

  url._extract_resource(source, start, end);
  url._extract_kvps(source, start, end);

  return url;
}

