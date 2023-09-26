#pragma once

#include "avr/uart/detail/math.hpp"
#include "avr/uart/detail/inline_asm.hpp"

#include <avr/io.hpp>
#include <stdint.h>

namespace avr::uart {

/**
   [optional] C++14 user defined literals to describe baud rates and
   clock frequencies.

   Examples:
   38400_bps
   576_Kbps
   1_Mbps

   1_MHz
   9600_KHz
 */
inline namespace literals {
  constexpr uint32_t operator""_bps(unsigned long long v) { return v; }
  constexpr uint32_t operator""_kbps(unsigned long long v) { return v * 1e3; }
  constexpr uint32_t operator""_Mbps(unsigned long long v) { return v * 1e6; }

  constexpr uint32_t operator""_Hz(unsigned long long v) { return v; }
  constexpr uint32_t operator""_kHz(unsigned long long v) { return v * 1e3; }
  constexpr uint32_t operator""_MHz(unsigned long long v) { return v * 1e6; }
}//namespace literals

/** CPU required cycles to handle transmission or reception of 1 bit. */
constexpr auto bit_length_cycles(uint32_t clk, uint32_t baud_rate)
{ return clk * 1.0/baud_rate; }

/**
   Represents a virtual UART device that uses software to transmit and
   receive bytes.

   How to use it?
   
   1. Instantiate an object of avr::uart::soft to set up the virtual
      device. For example:
      
        soft<Pb0, Pb1, 38400_bps, 8_MHz> uart;

      The construction will set up the Pb0 pin as an output pin and
      set a high level for Pb1 pin.

   2. Call the put() to transmit and the get() method to receive a
   byte.

   Arguments:

   TxPin: avrIO pin type describing the TX pin used by the application
          program. Example: avr::io::Pb0

   RxPin: avrIO pin type describing the RX pin used by the application
          program. Example: avr::io::Pb1

   baud_rate: unsigned value describing the baud rate
              speed. Optionally, C++14 user literals can be used for
              more expressiveness. Examples:

      soft<Pb0, Pb1, 38400> uart;
      soft<Pb0, Pb1, 38400_bps> uart;
      soft<Pb0, Pb1, 576000000_Kbps> uart;
      soft<Pb0, Pb1, 576_Kbps> uart;
      soft<Pb0, Pb1, 1000000> uart;
      soft<Pb0, Pb1, 1_Mbps> uart;

   clk_cpu: unsigned value describing CPU clock frequency. The F_CPU
            macro's value will be used by defaul if it is defined.
   
   If the macro F_CPU is used to define the CPU clock frequency, then
   there is no need to inform frequency in the construction of the
   virtual device. For example, the following snippet should be
   sufficient:

      //F_CPU defined
      
      soft<Pb0, Pb1, 38400_bps> uart;

   A clock be explicitly informed, for example:

      soft<Pb0, Pb1, 38400_bps, 8_MHz> uart;
   
 */
#ifdef F_CPU
template<typename TxPin, typename RxPin, uint32_t baud_rate, uint32_t clk_cpu = F_CPU>
#else
template<typename TxPin, typename RxPin, uint32_t baud_rate, uint32_t clk_cpu>
#endif
struct soft {
  using tx_pin = TxPin;
  using rx_pin = RxPin;
  static constexpr uint32_t bitrate = baud_rate;
  static constexpr uint32_t clk = clk_cpu;

  /** Rounded CPU cycles required to transmit/receive a byte. */
  static constexpr auto cycles_required{
    detail::math::round(bit_length_cycles(clk, bitrate))};

  static_assert(bit_length_cycles(clk, bitrate) >= 8,
    "the bit length in cycles must be greater or equal to 8. "\
    "[clk_frequency/baud_rate >= 8]");
  
  static_assert(bit_length_cycles(clk, bitrate) <= 513,
    "the bit length in cycles must be less than or equal to 513. "\
    "[clk_frequency/baud_rate <= 513]");
  
  /** Set up Tx pin as an output pin and set a high level on it. */
  soft() {
    TxPin::out();
    TxPin::high();
  }
  
  /** Transmit 1 byte through Tx. */
  void put(uint8_t byte) const {    
    /** loop instructions executed in 8 cycles */
    constexpr auto delay{cycles_required - 8};
      
    uint8_t port_value, bits;

    if constexpr (delay == 0) {
      asm volatile (
        AVR_UART_PUT_ASM_TMPL("", "")
        AVR_UART_PUT_OUT_OPS()
        AVR_UART_PUT_IN_OPS()
      );
    } else if constexpr (delay == 1) {
      asm volatile (
        AVR_UART_PUT_ASM_TMPL(AVR_UART_DELAY_1_CYCLE, "")
        AVR_UART_PUT_OUT_OPS()
        AVR_UART_PUT_IN_OPS()
      );
    } else if constexpr (delay == 2) {
      asm volatile (
        AVR_UART_PUT_ASM_TMPL(AVR_UART_DELAY_2_CYCLE, "")
        AVR_UART_PUT_OUT_OPS()
        AVR_UART_PUT_IN_OPS()
      );
    } else if constexpr (delay >= 3) {
      uint8_t delay_cnt, delay_b{delay / 3};
      if constexpr (delay % 3 == 0) {
        asm volatile (
          AVR_UART_PUT_ASM_TMPL(AVR_UART_DELAY_3_CYCLE, "")
          AVR_UART_PUT_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_PUT_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 1) {
        asm volatile (
          AVR_UART_PUT_ASM_TMPL(AVR_UART_DELAY_3_CYCLE, AVR_UART_DELAY_1_CYCLE)
          AVR_UART_PUT_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_PUT_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 2) {
        asm volatile (
          AVR_UART_PUT_ASM_TMPL(AVR_UART_DELAY_3_CYCLE, AVR_UART_DELAY_2_CYCLE)
          AVR_UART_PUT_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_PUT_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      }
    }
  }

  /**
     Receive and return 1 byte from Rx. This is a blocking call.

     Note that this method is not appropriate to receive a sequence of
     bytes in a row because some CPU cycles (from application code)
     will be consumed to handle each received byte, and this will
     delay the reception in a way that can be unfeasible to operate
     with a transmitter that sends a sequence without extra time
     between each sent byte.
  */
  uint8_t get() const {    
    /** loop instructions executed in 6 cycles */
    constexpr auto delay{cycles_required - 6};

    /** 4(or 3) cycles of instructions before reaching the point of
     * reading the bit. */
    constexpr auto one_half_delay
      {detail::math::round(1.5 * bit_length_cycles(clk, bitrate) - 4)};

    uint8_t byte{0}, bits, one_half_delay_cnt,
      one_half_delay_b{one_half_delay / 3};
    
    if constexpr (delay == 2 && one_half_delay % 3 == 0) {
      asm volatile(AVR_UART_GET_ASM_TMPL("",
                                         AVR_UART_DELAY_2_CYCLE,
                                         "")
        AVR_UART_GET_OUT_OPS()
        AVR_UART_GET_IN_OPS()
      );
    } else if constexpr (delay == 2 && one_half_delay % 3 == 1) {
      asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_1_CYCLE,
                                         AVR_UART_DELAY_2_CYCLE,
                                         "")
        AVR_UART_GET_OUT_OPS()
        AVR_UART_GET_IN_OPS()
        );
    } else if constexpr (delay == 2 && one_half_delay % 3 == 2) {
      asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_2_CYCLE,
                                         AVR_UART_DELAY_2_CYCLE,
                                         "")
        AVR_UART_GET_OUT_OPS()
        AVR_UART_GET_IN_OPS()
        );
    } else if constexpr (delay >= 3) {
      uint8_t delay_cnt, delay_b{delay / 3};
      if constexpr (delay % 3 == 0 && one_half_delay % 3 == 0) {
        asm volatile(AVR_UART_GET_ASM_TMPL("",
                                           AVR_UART_DELAY_3_CYCLE,
                                           "")
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 0 && one_half_delay % 3 == 1) {
        asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_1_CYCLE,
                                           AVR_UART_DELAY_3_CYCLE,
                                           "")
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 0 && one_half_delay % 3 == 2) {
        asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_2_CYCLE,
                                           AVR_UART_DELAY_3_CYCLE,
                                           "")
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 1 && one_half_delay % 3 == 0) {
        asm volatile(AVR_UART_GET_ASM_TMPL("",
                                           AVR_UART_DELAY_3_CYCLE,
                                           AVR_UART_DELAY_1_CYCLE)
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 1 && one_half_delay % 3 == 1) {
        asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_1_CYCLE,
                                           AVR_UART_DELAY_3_CYCLE,
                                           AVR_UART_DELAY_1_CYCLE)
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 1 && one_half_delay % 3 == 2) {
        asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_2_CYCLE,
                                           AVR_UART_DELAY_3_CYCLE,
                                           AVR_UART_DELAY_1_CYCLE)
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 2 && one_half_delay % 3 == 0) {
        asm volatile(AVR_UART_GET_ASM_TMPL("",
                                           AVR_UART_DELAY_3_CYCLE,
                                           AVR_UART_DELAY_2_CYCLE)
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 2 && one_half_delay % 3 == 1) {
        asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_1_CYCLE,
                                           AVR_UART_DELAY_3_CYCLE,
                                           AVR_UART_DELAY_2_CYCLE)
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      } else if constexpr (delay % 3 == 2 && one_half_delay % 3 == 2) {
        asm volatile(AVR_UART_GET_ASM_TMPL(AVR_UART_DELAY_2_CYCLE,
                                           AVR_UART_DELAY_3_CYCLE,
                                           AVR_UART_DELAY_2_CYCLE)
          AVR_UART_GET_OUT_OPS(AVR_UART_OUT_OPS_DELAY)
          AVR_UART_GET_IN_OPS(AVR_UART_IN_OPS_DELAY)
        );
      }
    }
    return byte;
  }
};

} //namespace avr::uart
