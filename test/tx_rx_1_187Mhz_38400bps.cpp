#define F_CPU 1187000

#include "tx_48bytes_rx_1byte.hpp"

int main()
{ test_for<38400_bps>(0x5a); }
