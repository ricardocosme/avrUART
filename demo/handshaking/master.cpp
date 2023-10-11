#include <avr/uart.hpp>
#include <avr/sleep.hpp>

using namespace avr::io;
using namespace avr::sleep;
using namespace avr::uart;

using Tx = Pb0;
using Rx = Pb1;

AVRINT_WDT() {}

int main() {
  osccal = 0x9a;
  constexpr auto baud_rate = 57600_bps;
  constexpr auto clk = 8_MHz;
  
  avr::uart::soft<Tx, Rx, baud_rate, clk> uart;
  avr::uart::soft<Pb4, Pb3, baud_rate, clk> term;
  
  while(true) {
    power_down::sleep_for<500_ms>();

    //handshaking
    uart.request_to_send();
    
    //send the command
    uart.put(0x4e);

    //receive the data
    auto data = uart.get_bytes<3>();
    for(auto b : data)
      term.put(b);
  }
}
