#include "configuration.h"

#include "ini-file.h"

using webon::Configuration;
using webon::address::IPv4;
using webon::address::Port16;

std::string Configuration::Default_Root_Location{"/www/default"};
IPv4 Configuration::Default_Ip_Address{};
Port16 Configuration::Default_Port{80};

Configuration::Configuration(string const& www_root, IPv4 ip_address, Port16 port):
  _www_root{www_root},
  _ip_address{ip_address},
  _port{port}
{}

Configuration::Configuration(IPv4 ip_address, Port16 port):
  Configuration(Default_Root_Location, ip_address, port)
{}

Configuration::Configuration(IPv4 ip_address):
  Configuration(Default_Root_Location, ip_address, Default_Port)
{}

Configuration::Configuration(Port16 port):
  Configuration(Default_Root_Location, Default_Ip_Address, port)
{}

Configuration::Configuration(Ini_File const& ini_file):
  Configuration()
{
  string const* value = nullptr;

  if ((value = ini_file.Get_Value("port")) != nullptr)
    _port = stoi(*value);

  if ((value = ini_file.Get_Value("www_root")) != nullptr)
    _www_root = std::move(*value);
}


Configuration::Configuration():
  Configuration(Default_Root_Location, Default_Ip_Address, Default_Port)
{}


