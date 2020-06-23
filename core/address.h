#ifndef WEBON__ADDRESS_H__INCLUDED
#define WEBON__ADDRESS_H__INCLUDED

#include <cstdint>
#include <list>
#include <string>
#include <thread>

#include <netinet/in.h>

namespace webon
{
  namespace address
  {
    using std::string;

    inline constexpr uint32_t makeui32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
      return (uint32_t(a) << 24) | (b << 16) | (c << 8) | d;
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

