#include <avr/uart.hpp>
#include <avr/sleep.hpp>
#define F_CPU 1000000
#include <util/delay.h>

using namespace avr::interrupt;
using namespace avr::io;
using namespace avr::sleep;
using namespace avr::uart;

using Tx = Pb0;
using Rx = Pb1;

int main() {
  osccal = 0x9d;
  
  avr::uart::soft<Tx, Rx, 38400_bps, 1_MHz> uart;
  avr::uart::soft<Pb4, Pb3, 38400_bps, 1_MHz> term;
  
  while(true) {
    _delay_ms(500);

    //send a request to send data
    Tx::low();
    while(Rx::is_high());
    Tx::high();
    _delay_us(5);

    //send the command
    uart.put(0x4e);

    //receive the data
    auto data = uart.get_bytes<3>();
    for(auto b : data)
      term.put(b);
  }
}
