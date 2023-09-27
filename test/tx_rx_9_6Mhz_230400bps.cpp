#include "tx_48bytes_rx_1byte.hpp"

int main()
{ test_for<9600_kHz, 230400_bps>(0x5a); }
