#pragma once

#include <avr/io.hpp>
#include <avr/uart.hpp>

using namespace avr::uart::literals;

template<uint32_t baud_rate, uint32_t clk>
inline void test_for(uint8_t osccal_p) {
  using namespace avr::io;
  
  osccal = osccal_p;
  
  avr::uart::soft<Pb4/*tx*/, Pb3/*rx*/, baud_rate, clk> uart;

  while(true) {
    auto values = uart.template get<48>();
    for(uint8_t c : values)
      uart.put(c);
  }
}
