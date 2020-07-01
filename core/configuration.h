#ifndef WEBON__CONFIGURATION_H__INCLUDED
#define WEBON__CONFIGURATION_H__INCLUDED

#include <cstdint>

#include "address.h"

namespace webon
{
  class Ini_File;

  class Configuration
  {
    private:
      static string Default_Root_Location;
      static address::IPv4 Default_Ip_Address;
      static address::Port16 Default_Port;

      string _www_root;
      address::IPv4 _ip_address;
      address::Port16 _port;

    public:
      Configuration(string const& www_root, address::IPv4 ip_address, address::Port16 port);
      Configuration(address::IPv4 ip_address, address::Port16 port);
      Configuration(address::IPv4 ip_address);
      Configuration(address::Port16 port);
      Configuration(Ini_File const& ini_file);
      Configuration();

      Configuration(uint16_t port):
        Configuration("/www/default", 0, port)
      {}

      string const& www_root() const { return _www_root; }
      address::IPv4 ip_address() const { return _ip_address; }
      address::Port16 port() const { return _port; }
  };
}  // namespace webon

#endif  // WEBON__CONFIGURATION_H__INCLUDED

