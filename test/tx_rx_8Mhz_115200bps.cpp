#include "tx_48bytes_rx_1byte.hpp"

int main()
{ test_for<8_MHz, 115'200_bps>(0x9a); }
