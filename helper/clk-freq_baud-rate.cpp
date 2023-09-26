#include "avr/uart/detail/math.hpp"

#include <iostream>
#include <string>
#include <stdint.h>

using namespace avr::uart::detail;

constexpr auto bit_length_cycles(uint32_t clk, uint32_t baud_rate)
{ return clk*1.0/baud_rate; }

constexpr auto bit_length_cycles_error(uint32_t clk, uint32_t baud_rate) {
  auto c = clk*1.0/baud_rate;
  return (math::abs(c - math::round(c)) / c) * 100;
}

constexpr auto one_half_bit_length_cycles_error(uint32_t clk, uint32_t baud_rate) {
  auto c = clk*1.5/baud_rate;
  return (math::abs(c - math::round(c)) / c) * 100;
}

int main(int argc, char** argv) {
  using namespace std;
  
  if(argc != 3) {
    cout
      << "usage: clk-freq_baud-rate <cpu_clock_hz> <bps>"
      << endl
      << "example: clk-freq_baud-rate 8000000 38400"
      << endl;
    return 1;
  }
  auto clk = stoul(argv[1]);
  auto baud_rate = stoul(argv[2]);
  
  cout
    << "CPU frequency: " << clk << " Hz" << endl
    << "Baud rate: " << baud_rate << " bps" << endl << endl;
  
  cout
    << "Bit length in CPU cycles (c): " << clk*1.0/baud_rate << " cycles" << endl
    << "Rounded bit length in CPU cycles (rc): " << unsigned(math::round(clk*1.0/baud_rate))
    << " cycles" << endl
    << " relative error (|c - rc|)/c: " << bit_length_cycles_error(clk, baud_rate) << " %" << endl
    << endl;

  cout
    << "1.5 Bit length in CPU cycles (c): " << 1.5 * bit_length_cycles(clk, baud_rate) << " cycles" << endl
    << "Rounded 1.5 bit length in CPU cycles (rc): " << unsigned(math::round(1.5 * bit_length_cycles(clk, baud_rate)))
    << " cycles" << endl
    << " relative error (|c - rc|)/c: " << one_half_bit_length_cycles_error(clk, baud_rate) << " %" << endl
    << endl;
}
