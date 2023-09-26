MCU:=attiny13a
DEV:=t13
AVR_IO_INCLUDE=$(HOME)/avrIO/include

CXX=avr-g++
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
INCLUDE=-I. -I../include -I$(AVR_IO_INCLUDE)
CXXFLAGS=-std=c++17 -mmcu=$(MCU) -Wall -Os $(INCLUDE) \
  -Wno-unused-variable -Wno-unused-but-set-variable -Wno-array-bounds

all: 

%.elf: %.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

.PHONY: flash_%
flash-%: %.hex
	avrdude -p $(DEV) -c usbasp -P usb -U flash:w:$<

.PHONY: clean
clean:
	rm -f *.hex *.lst *.elf *.o *.s
