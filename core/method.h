#ifndef WEBON__METHOD_H__INCLUDED
#define WEBON__METHOD_H__INCLUDED

namespace webon
{
  using std::string;
  using std::string_view;
  using std::vector;

  enum class Method
  {
    OPTIONS,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT
  };

  inline constexpr string_view Get_Method_Name(Method method)
  {
    switch (method)
    {
      case Method::OPTIONS: return "OPTIONS";
      case Method::GET: return "GET";
      case Method::HEAD: return "HEAD";
      case Method::POST: return "POST";
      case Method::PUT: return "PUT";
      case Method::DELETE: return "DELETE";
      case Method::TRACE: return "TRACE";
      case Method::CONNECT: return "CONNECT";
    }

    return "";
  }
}  // namespace webon

#endif  // WEBON__METHOD_H__INCLUDED

