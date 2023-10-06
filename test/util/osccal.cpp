#include <avr/io.hpp>

using namespace avr::io;

int main() {
  osccal = 0x5a;
  out(pb1);
  while(true)
    pb1.toggle();
}
