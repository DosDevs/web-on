#include <fstream>
#include <iostream>
#include <memory>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "address.h"
#include "configuration.h"
#include "httpd.h"
#include "ini-file.h"
#include "utility.h"

std::unique_ptr<webon::httpd> Global_Httpd;

void signalHandler(int signal)
{
  std::cout << "Shutting down..." << std::endl;

  if (!Global_Httpd)
    return;

  Global_Httpd->Stop();
  exit(1);
}

int main()
{
//  std::string const url_string { R"(https://www.amazon.com/10Gtek-SFP-10G-T-S-Compatible-10GBase-T-Transceiver/dp/B01KFBFL16/ref=sr_1_4?crid=KUFSOZMZ7KNC&dchild=1&keywords=sfp%2B+ethernet+10g&qid=1593800604&sprefix=sfp%2B+ether%2Caps%2C199&sr=8-4)" };
//  auto url { webon::address::URL::Parse(url_string) };
//
//  SHOW(url_string);
//  SHOW(url.Get_Namespace());
//  SHOW(url.Get_Name());
//  SHOW(url.Get_Host());
//  SHOW(url.Get_Port());
//  SHOW(url.Get_Resource());
//  SHOW(url.Get_Kvps().size());
//
//  for (auto const& kvp: url.Get_Kvps())
//    std::cout << kvp.first << ": " << kvp.second << std::endl;

  std::ifstream config_file_stream{"webon.ini"};
  webon::Ini_File ini_file;

  config_file_stream >> ini_file;
  if (ini_file.Is_Empty())
  {
    std::cout << "Cannot read configuration." << std::endl;
    return 0;
  }

  webon::Configuration configuration{ini_file};
  Global_Httpd = std::make_unique<webon::httpd>(std::move(configuration));

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = signalHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  if (int r = Global_Httpd->Start(); r != 0)
  {
    std::cout << "There was an error starting web server: " << strerror(errno) << " (" << errno << ")" << "." << std::endl;
    return r;
  }

  return 0;
}

