#define F_CPU 8000000

#include "tx_48bytes_rx_1byte.hpp"

int main()
{ test_for<460'800_bps>(0x9a); }
