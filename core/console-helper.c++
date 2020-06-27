#include "console-helper.h"

#include <sys/select.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

namespace {
  termios ttystate;
  termios ttysave;
}  // namespace

void console::Initialize()
{
  tcgetattr(STDIN_FILENO, &ttystate);
  ttysave = ttystate;

  ttystate.c_lflag &= ~(ICANON | ECHO);
  ttystate.c_cc[VMIN] = 1;

  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int console::Check_For_Key_Press()
{
  fd_set readset;
  FD_ZERO(&readset);
  FD_SET(fileno(stdin), &readset);

  timeval tv {
    .tv_sec = 1,
    .tv_usec = 0
  };

  select(fileno(stdin) + 1, &readset, nullptr, nullptr, &tv);

  if(FD_ISSET(fileno(stdin), &readset))
  {
    return getchar();
  }

  return 0;
}

void console::Uninitialize()
{
  ttystate.c_lflag |= ICANON | ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
}

