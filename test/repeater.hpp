#pragma once

#include <avr/io.hpp>
#include <avr/uart.hpp>

using namespace avr::uart::literals;

template<uint32_t clk, uint32_t src_baud_rate, uint32_t dst_baud_rate = src_baud_rate>
inline void test_for(uint8_t osccal_p) {
  using namespace avr::io;
  
  osccal = osccal_p;
  
  avr::uart::soft<Pb0/*tx*/, Pb1/*rx*/, src_baud_rate, clk> src;
  avr::uart::soft<Pb4/*tx*/, Pb3/*rx*/, dst_baud_rate, clk> dst;
  
  while(true) {
    uint8_t bytes[48];
    for(int i{0}; i < 48; ++i)
      bytes[i] = src.get();
    for(auto b : bytes)
      dst.put(b);
  }
}
