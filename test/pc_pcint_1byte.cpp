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

#define  BPS B38400

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
  
  std::cout << "ready to receive" << std::endl;

  uint8_t byte{0};
  while(true) {
    std::this_thread::sleep_for(100ms);
    
    write(fd, &byte, 1);
    uint8_t c;
    read(fd, &c, 1);
    print(c);
    if(c != byte) abort();
    if(byte++ == 49) byte = 0;
  }
  close(fd);
}
