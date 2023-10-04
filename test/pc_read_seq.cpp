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

// #define  BPS B9600
// #define  BPS B19200
#define  BPS B38400
// #define  BPS B57600
// #define  BPS B115200
// #define  BPS B230400
// #define  BPS B460800
// #define  BPS B500000
// #define  BPS B576000
// #define  BPS B921600

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

  const uint8_t n_bytes{48};
  uint8_t bytes[n_bytes];
  
  uint8_t send[n_bytes];
  for(uint8_t i{0}; i < 48; ++i)
    send[i] = i;
  
  while(true) {
    std::this_thread::sleep_for(500ms);
    
    write(fd, send, 48);
    
    for(auto& c : bytes){
      read(fd, &c, 1);
      print(c);
    }
    for(int n{0}; n < n_bytes; ++n)
      if(bytes[n] != n) abort();
  }
  close(fd);
}
