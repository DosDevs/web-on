#ifndef WEBON__UTILITY_H__INCLUDED
#define WEBON__UTILITY_H__INCLUDED

#define ever (;;)

#define SHOW(N)                                 \
  std::cout << #N << ": <" << (N) << ">" << std::endl;  \

namespace webon
{
  using std::string_view;

  constexpr bool String_Begins_With(string_view a, string_view b)
  {
    return ((a.length() > b.length()) && (a.substr(0, b.length()) == b));
  }

  inline constexpr char CR = 13;
  inline constexpr char LF = 10;
  inline constexpr char SP = 32;
  inline constexpr char HT = 9;
  inline constexpr char QM = 34;
  inline constexpr char DEL = 127;

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

}  // namespace webon

#endif  // WEBON__UTILITY_H__INCLUDED

