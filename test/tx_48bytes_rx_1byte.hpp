#pragma once

#include <avr/io.hpp>
#include <avr/uart.hpp>

using namespace avr::uart::literals;

template<uint32_t clk, uint32_t baud_rate>
inline void test_for(uint8_t osccal_p) {
  using namespace avr::io;
  
  osccal = osccal_p;

  avr::uart::soft<Pb4/*tx*/, Pb3/*rx*/, baud_rate, clk> uart;

  while(true) {
    for(int b{0}; b < 48; ++b) 
      uart.put(b);
    uart.put(uart.get());
  }
}
