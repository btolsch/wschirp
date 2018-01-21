#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <string>

int main() {
  struct timeval timeout{0, 500};
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);
  if (select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout) != 1) {
    return 0;
  }
  std::string line;
  std::getline(std::cin, line);
  std::cout << line << std::endl;
  return 0;
}
