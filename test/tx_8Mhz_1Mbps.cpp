#define F_CPU 8000000 /*8 MHz*/

#include "tx_48bytes.hpp"

int main()
{ test_for<1_Mbps>(0x80); }
