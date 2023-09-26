#pragma once

#include <stdint.h>

namespace avr::uart::detail::math {

constexpr uint8_t round(double n) {
  uint8_t whole = n;
  return (n - whole >= 0.5) ? whole + 1 : whole;
}

template<typename N>
constexpr auto abs(N n)
{ return n < 0 ? n * -1 : n; }

}
