#/bin/sh
./make-t13a.sh -j8 -B \
    tx_rx_1_187Mhz_38400bps.s \
    tx_rx_9_6Mhz_230400bps.s \
    tx_rx_9_6Mhz_460800bps.s

./make-t85.sh -j8 -B \
    repeater_8Mhz_1Mbps.s \
    tx_8Mhz_1Mbps.s \
    tx_rx_8Mhz_576kbps.s \
    tx_rx_8Mhz_500kbps.s \
    tx_rx_8Mhz_460800bps.s \
    tx_rx_8Mhz_230400bps.s \
    tx_rx_8Mhz_115200bps.s \
    tx_rx_1Mhz_57600bps.s \
    tx_rx_1Mhz_38400bps.s \
    tx_rx_1Mhz_19200bps.s \
    tx_rx_1Mhz_9600bps
