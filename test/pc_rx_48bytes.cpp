#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <string>
#include <iostream>
#include <format>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

void print(uint8_t byte)
{ std::cout << std::format("{:#04x} ", byte) << std::flush; }

#define BPS B921600

int main() {
  struct termios settings{};

  settings.c_cflag = CREAD | CLOCAL | CS8;

  settings.c_cc[VMIN] = 1;
  settings.c_cc[VTIME] = 0;

  auto fd = open("/dev/ttyUSB0", O_RDWR);
  if (fd == -1)
    return -1;

  cfsetospeed(&settings, BPS);
  cfsetispeed(&settings, BPS);
  
  tcsetattr(fd, TCSANOW, &settings);
  
  uint8_t cnt{0};
  uint8_t n_bytes{48};

  std::cout << "ready to receive" << std::endl;
  
  while(true) {
    uint8_t bytes[n_bytes];
    for(int n{0}; n < n_bytes; ++n) {
      read(fd, &bytes[n], 1);
      print(bytes[n]);
    }
    for(int n{0}; n < n_bytes; ++n)
      if(bytes[n] != n) abort();
  }
  close(fd);
}
