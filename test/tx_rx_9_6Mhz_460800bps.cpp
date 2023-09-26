#define F_CPU 9600000

#include "tx_48bytes_rx_1byte.hpp"

int main()
{ test_for<460'800_bps>(0x5a); }
