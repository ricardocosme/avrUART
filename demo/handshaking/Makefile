MCU:=attiny13a
DEV:=t13

AVR_IO_INCLUDE=$(HOME)/avrIO/include
AVR_SLEEP_INCLUDE=$(HOME)/avrSLEEP/include
AVR_WDT_INCLUDE=$(HOME)/avrWDT/include
AVR_INT_INCLUDE=$(HOME)/avrINT/include

CXX=avr-g++
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
INCLUDE=-I../../include -I$(AVR_IO_INCLUDE) -I$(AVR_SLEEP_INCLUDE) -I$(AVR_WDT_INCLUDE) -I$(AVR_INT_INCLUDE)
CXXFLAGS=-std=c++17 -mmcu=$(MCU) -Wall -Os $(INCLUDE)  -Wno-array-bounds

all: slave.lst master.lst

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
