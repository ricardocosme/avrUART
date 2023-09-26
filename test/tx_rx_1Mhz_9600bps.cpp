#include "tx_48bytes_rx_1byte.hpp"

int main()
{ test_for<1_MHz, 9600_bps>(0x9d); }
