#include <avr/uart.hpp>
#include <avr/sleep.hpp>
#define F_CPU 1200000
#include <util/delay.h>

using namespace avr::interrupt;
using namespace avr::io;
using namespace avr::sleep;
using namespace avr::uart;

bool request_to_send{false};

AVRINT_PCINT0() { request_to_send = true; }

using Tx = Pb4;
using Rx = Pb3;

int main() {
  osccal = 0x5a;
  
  avr::uart::soft<Tx, Rx, 38400_bps, 1200_kHz> uart;
  
  set(pcie);
  set(pcint3); //Rx
  avr::interrupt::on();

  while(true) {
    power_down::sleep();
    {
      atomic scope{on_at_the_end};
      if(request_to_send) {
        request_to_send = false;
        
        Tx::low();
        while(Rx::is_low());
        Tx::high();

        if(uart.get() == 0x4e) {
          uart.put(0x01);
          uart.put(0x02);
          uart.put(0x03);
        }
      }
    }
  }
}
