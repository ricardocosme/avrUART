#include <avr/uart.hpp>

using namespace avr::io;
using namespace avr::uart::literals;

int main() {
  avr::uart::soft<Pb0/*tx*/, Pb1/*rx*/, 38400_bps, 1_MHz> uart;
  while(true)
    uart.put(uart.get());
}
