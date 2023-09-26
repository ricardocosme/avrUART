#pragma once

#include <avr/io.hpp>
#include <avr/uart.hpp>
#include <util/delay.h>

using namespace avr::uart::literals;

template<uint32_t baud_rate>
inline void test_for(uint8_t osccal_p) {
  using namespace avr::io;
  
  osccal = osccal_p;
  
  avr::uart::soft<Pb4/*tx*/, Pb3/*rx*/, baud_rate> uart;
  
  while(true) {
    _delay_ms(500);
    for(int b{0}; b < 48; ++b)  {
      uart.put(b);
      __builtin_avr_delay_cycles(100);
    }
  }
}
