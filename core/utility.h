#ifndef WEBON__UTILITY_H__INCLUDED
#define WEBON__UTILITY_H__INCLUDED

#include <fstream>
#include <iostream>

#include <unistd.h>

#define ever (;;)

#define SHOW(N)                                         \
  std::cout << #N << ": <" << (N) << ">" << std::endl;  \

namespace webon
{
  using std::string;
  using std::string_view;

  inline constexpr char CR = 13;
  inline constexpr char LF = 10;
  inline constexpr char SP = 32;
  inline constexpr char HT = 9;
  inline constexpr char QM = 34;
  inline constexpr char DEL = 127;
  inline constexpr char SPACES[] = { SP, HT, 0 };
  inline constexpr char CRLF[] = { CR, LF, 0 };

  inline constexpr bool is_char(int value)
  {
    return ((value >= 0) && (value <= 127));
  }

  inline constexpr bool is_upalpha(int value)
  {
    return ((value >= 'A') && (value <= 'Z'));
  }

  inline constexpr bool is_loalpha(int value)
  {
    return ((value >= 'a') && (value <= 'z'));
  }

  inline constexpr bool is_alpha(int value)
  {
    return (is_loalpha(value) || is_upalpha(value));
  }

  inline constexpr bool is_digit(int value)
  {
    return ((value >= '0') && (value <= '9'));
  }

  inline constexpr bool is_hex(int value)
  {
    return (is_digit(value) || ((value >= 'A') && (value <= 'F')) || ((value >= 'a') && (value <= 'f')));
  }

  inline constexpr bool is_control(int value)
  {
    return (((value >= 0) && (value <= 31)) || (value == 127));
  }

  inline constexpr bool is_cr(int value)
  {
    return value == CR;
  }

  inline constexpr bool is_lf(int value)
  {
    return value == LF;
  }

  inline constexpr bool is_space(int value)
  {
    return value == SP;
  }

  inline constexpr bool is_ht(int value)
  {
    return value == HT;
  }

  inline constexpr bool is_quote(int value)
  {
    return value == QM;
  }

  inline constexpr bool is_separator(int value)
  {
    switch (value)
    {
      case '(':
      case ')':
      case '<':
      case '>':
      case '@':
      case ',':
      case ';':
      case ':':
      case '\\':
      case '"':
      case '/':
      case '[':
      case ']':
      case '?':
      case '=':
      case '{':
      case '}':
      case SP:
      case HT:
        return true;
    }

    return false;
  }

  inline bool File_Is_Accesible(string const& file)
  {
    return (access(file.c_str(), F_OK) == 0);
  }

  inline string Read_File(string const& file)
  {
    string contents{};
    std::ifstream stream{file};

    if (!stream)
      return contents;

    if (!stream.seekg(0, std::ios_base::end))
      return contents;

    auto file_size = stream.tellg();

    if (!stream.seekg(0, std::ios_base::beg))
      return contents;

    contents.resize(file_size);

    if (!stream.read(&contents[0], file_size))
      contents.clear();

    return contents;
  }

  inline constexpr bool String_Begins_With(string_view a, string_view b)
  {
    return ((a.length() >= b.length()) && (a.substr(0, b.length()) == b));
  }

  inline constexpr bool String_Ends_With(string_view a, string_view b)
  {
    return ((a.length() >= b.length()) && (a.substr(a.length() - b.length()) == b));
  }

  inline constexpr string_view trim(string_view s)
  {
    auto const first = s.find_first_not_of(SPACES);
    auto const last = s.find_last_not_of(SPACES);

    if ((first == string::npos) || (last == string::npos))
      return "";

    return s.substr(first, last - first + 1);
  }
}  // namespace webon

#endif  // WEBON__UTILITY_H__INCLUDED

