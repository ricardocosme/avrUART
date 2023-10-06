#include <avr/uart.hpp>
#include <avr/sleep.hpp>

using namespace avr::io;
using namespace avr::interrupt;
using namespace avr::sleep;
using namespace avr::uart;

/**
   Wake-up the MCU from power down to handle one command sent through
   UART.

   Warning: this approach is just one of several alternatives. It
   doesn't require an exclusive line (pin) for flow control or
   handshake, but it can be tricky because the time after sending the
   start bit must be taken into account when calling get(). This
   requires a good understanding of what is happening and a commitment
   to the generated code, because the code region after the wake-up is
   time-sensitive, and the compiler may inject instructions there
   that need to be considered.
 */

AVRINT_PCINT0() {}

int main() {
  osccal = 0x5a;

  avr::uart::soft<Pb4/*tx*/, Pb3/*rx*/, 38400_bps, 1200_kHz> uart;

  /** Set-up pin change at Pb3 as a wake-up source: the device will
   * wake-up when a start bit is detected on the line to transmit one
   * command (one byte). */
  set(pcie);
  set(pcint3);
  avr::interrupt::on();
  
  while(true) {
    power_down::sleep();

    /** we don't want to be interrupted when reading or sending
     * through UART as these operations are time-sensitive. */
    atomic scope{on_at_the_end};
    
    /**
       Read the command.
       
       This is just an example using ATtiny13A. This type of solution
       requires a good understanding of what is going on during the
       interrupt process and also how get() works in general.
       
         4 //after sleep
       + 6 //start-up time from power down
       + 2 //rjmp from the vector table
       + 4 //reti from the interrupt function
       + 5 //instructions executed before reaching the busy wait for
           //the start bit. [*]
       = 21

       [*] it must considers the generated code.
    */
    auto cmd = uart.get<21_cycles_ago>();

    /** echoes the received command. */
    uart.put(cmd);
  }
}
