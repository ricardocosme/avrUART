#include "read_seq.hpp"

int main()
{ test_for<115'200_bps, 8_MHz>(0x80); }

  // osccal = 0x9a; // 8 MHz
  avr::uart::soft<Pb4, Pb3, 115'200_bps, 8_MHz> uart;
  // osccal = 0xa6; // 8.5 MHz
  // avr::uart::soft<Pb4, Pb3, 500_kbps, 8500_kHz> uart;
  while(true) {
    auto values = uart.get<48>();
    for(uint8_t c : values)
      uart.put(c);
    // uint8_t bytes[48];
    // for(uint8_t i{0}; i < sizeof(bytes); ++i)
    //   bytes[i] = uart.get(); //get() is a blocking call that receives 1 byte
    // for(uint8_t i{0}; i < sizeof(bytes); ++i)
    // uart.put(bytes[i]);
  }
}
