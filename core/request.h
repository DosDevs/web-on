#ifndef WEBON__REQUEST_H__INCLUDED
#define WEBON__REQUEST_H__INCLUDED

#include <string>
#include <vector>

namespace webon
{
  using std::string;
  using std::vector;

  class Request
  {
    private:
      vector<string> _rep;

    public:
      constexpr Request& operator+=(string&& line)
      {
        _rep.push_back(std::move(line));

        return *this;
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
}  // namespace webon

#endif  // WEBON__REQUEST_H__INCLUDED

