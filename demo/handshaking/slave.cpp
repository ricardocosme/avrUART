#include <avr/uart.hpp>
#include <avr/sleep.hpp>

using namespace avr::interrupt;
using namespace avr::io;
using namespace avr::sleep;
using namespace avr::uart;

volatile bool request_to_send{false};

AVRINT_PCINT0() { request_to_send = true; }

using Tx = Pb4;
using Rx = Pb3;

int main() {
  osccal = 0x5a;
  
  avr::uart::soft<Tx, Rx, 57600_bps, 1200_kHz> uart;
  
  set(pcie);
  set(pcint3); //Rx
  avr::interrupt::on();

  while(true) {
    power_down::sleep();
    
    if(request_to_send) {
      atomic scope{on_at_the_end};
      request_to_send = false;

      //handshaking
      uart.clear_to_send();
      
      auto cmd = uart.get();
      
      if(cmd == 0x4e) {
        uart.put(0x01);
        uart.put(0x02);
        uart.put(0x03);
      }
    }
  }
}
