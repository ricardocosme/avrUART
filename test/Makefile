AVR_IO_INCLUDE=$(HOME)/avrIO/include

CXX=avr-g++
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
INCLUDE=-I. -I../include -I$(AVR_IO_INCLUDE)
CXXFLAGS=-std=c++17 -mmcu=$(MCU) -Wall -Os $(INCLUDE) \
  -Wno-unused-variable -Wno-unused-but-set-variable -Wno-array-bounds

all:

pc_rx_48bytes_tx_1byte:
	g++ -std=c++20 -O3 -o pc_rx_48bytes_tx_1byte pc_rx_48bytes_tx_1byte.cpp

pc_rx_48bytes:
	g++ -std=c++20 -O3 -o pc_rx_48bytes pc_rx_48bytes.cpp

pc_read_seq:
	g++ -std=c++20 -O3 -o pc_read_seq pc_read_seq.cpp

pc_pcint_1byte:
	g++ -std=c++20 -O3 -o pc_pcint_1byte pc_pcint_1byte.cpp

%.s: %.cpp
	$(CXX) $(CXXFLAGS) -S $^

%.elf: %.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

.PHONY: flash_%
flash-%: %.hex
	avrdude -p $(DEV) -c usbasp -P usb -U flash:w:$<

.PHONY: clean
clean:
	rm -f *.hex *.lst *.elf *.o *.s
