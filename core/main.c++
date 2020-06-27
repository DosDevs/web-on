#include <iostream>
#include <memory>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "httpd.h"

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
  Global_Httpd = std::make_unique<webon::httpd>(webon::address::Port16{8080});

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

