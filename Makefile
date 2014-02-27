CLOCK = 48000000

TEENSY_PATH = ~/498/teensy-toolchain
COMPILER = $(TEENSY_PATH)/hardware/tools/arm-none-eabi/bin
VENDOR = ./vendor/


CPPFLAGS = -Wall -g -Os -mcpu=cortex-m4 -mthumb -nostdlib -MMD -DF_CPU=$(CLOCK) -DUSB_SERIAL -I$(VENDOR) -D__MK20DX256__
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti
CFLAGS =
LDFLAGS = -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb -T$(VENDOR)/mk20dx256.ld
LIBS = -lm
CC = $(COMPILER)/arm-none-eabi-gcc
CXX = $(COMPILER)/arm-none-eabi-g++
OBJCOPY = $(COMPILER)/arm-none-eabi-objcopy
SIZE = $(COMPILER)/arm-none-eabi-size

OBJECTS = timer_test.o

VENDOR_C = $(wildcard $(VENDOR)/*.c)
VENDOR_OBJECTS = $(patsubst %.c,%.o,$(VENDOR_C))

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@


timer_test.elf: $(OBJECTS) $(VENDOR_OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(VENDOR_OBJECTS) $(LIBS) 

-include $(OBJS:.o=.d)

all: test.hex

clean:
	rm -f *.o *.d *.elf *.hex

