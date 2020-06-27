#ifndef WEBON__ADDRESS_H__INCLUDED
#define WEBON__ADDRESS_H__INCLUDED

#include <cstdint>
#include <list>
#include <string>
#include <thread>

#include <netinet/in.h>

namespace webon
{
  using std::string;

  namespace address
  {
    inline constexpr uint32_t makeui32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
      return (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(c) << 8) | uint32_t(d);
    }

    inline constexpr int get_octet(uint32_t value, int i)
    {
      if ((i < 0) || (i > 3))
        return 0;

      return ((value >> (i * 8)) & 0xff);
    }

    class IPv4
    {
      private:
        uint32_t _rep;

      public:
        constexpr IPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d): _rep(makeui32(a, b, c, d)) {}

        constexpr IPv4(uint32_t value): _rep(value) {}

        constexpr IPv4(): IPv4(0) {}

        constexpr operator uint32_t() const { return _rep; }

        operator string() const {
          int big_endian = htonl(_rep);

          return
              std::move(
                  std::to_string(big_endian & 0xff) + string(".") +
                  std::to_string((big_endian >> 8) & 0xff) + string(".") +
                  std::to_string((big_endian >> 16) & 0xff) + string(".") +
                  std::to_string((big_endian >> 24) & 0xff));
        }
    };

    template<typename Stream>
    inline constexpr Stream& operator<<(Stream& stream, IPv4 const& address)
    {
      int big_endian = htonl(address);

      return (
          stream << get_octet(big_endian, 0) << "."
                 << get_octet(big_endian, 1) << "."
                 << get_octet(big_endian, 2) << "."
                 << get_octet(big_endian, 3));
    }

    class Port16
    {
      private:
        uint16_t _rep;

      public:
        constexpr Port16(uint16_t value): _rep(value) {}

        constexpr Port16(): Port16(0) {}

        constexpr operator uint16_t() const { return _rep; }

        operator string() const {
          return std::move(std::to_string(_rep));
        }
    };
  }  // namespace address
}  // namespace webon

#endif  // WEBON__ADDRESS_H__INCLUDED

