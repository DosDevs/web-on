#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "httpd.h"

webon::httpd *Global_Httpd;

void signalHandler(int signal)
{
  std::cout << "Shutting down..." << std::endl;
  Global_Httpd->Close();
  exit(1);
}

int main()
{
  webon::httpd httpd{webon::address::Port16{8080}};
  Global_Httpd = &httpd;

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = signalHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  if (int r = httpd.Start(); r != 0)
  {
    std::cout << "There was an error starting web server: " << strerror(r) << " (" << r << ")" << "." << std::endl;
    return r;
  }

  return 0;
}

