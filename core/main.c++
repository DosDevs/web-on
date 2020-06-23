#include <iostream>
#include <stdio.h>

#include "httpd.h"

int main()
{
  webon::httpd httpd{webon::address::IPv4{}, webon::address::Port16{80}};

  if (int r = httpd.Start(); r != 0)
  {
    std::cout << "There was an error starting web server: " << r << "." << std::endl;
    return r;
  }

  return 0;
}

