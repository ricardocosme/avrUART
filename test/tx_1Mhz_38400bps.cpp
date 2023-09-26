#define F_CPU 1000000

#include "tx_48bytes.hpp"

int main()
{ test_for<38400_bps>(0x9b); }
