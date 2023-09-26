#include <avr/io.hpp>

using namespace avr::io;

int main() {
  osccal = 0x9f;
  out(pb1);
  while(true)
    pb1.toggle();
}
