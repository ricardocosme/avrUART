#include <avr/uart.hpp>

using namespace avr::io;
using namespace avr::uart::literals;

int main() {
  avr::uart::soft<Pb0/*tx*/, Pb1/*rx*/, 38400_bps, 1_MHz> uart;
  while(true) {
    /** Wait for a reception of 10 bytes. */
    auto bytes = uart.get_bytes<10>();
    
    for(auto c : bytes)
      uart.put(c);
  }
}
