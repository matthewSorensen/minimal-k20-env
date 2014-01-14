# configurable options
OPTIONS = -DF_CPU=48000000 -DUSB_SERIAL
# options needed by many Arduino libraries to configure for Teensy 3.0
OPTIONS += -D__MK20DX128__

TEENSY_PATH = /home/matthew/498/teensy-toolchain
TOOLS = $(TEENSY_PATH)/hardware/tools
COMPILER = $(TOOLS)/arm-none-eabi/bin

CPPFLAGS = -Wall -g -Os -mcpu=cortex-m4 -mthumb -nostdlib -MMD $(OPTIONS) -I/home/matthew/498/teensy-toolchain/hardware/teensy/cores/teensy3
# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti
# compiler options for C only
CFLAGS =
# linker options
LDFLAGS = -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb -T$(VENDOR)/mk20dx128.ld
# additional libraries to link
LIBS = -lm
# names for the compiler programs
CC = $(COMPILER)/arm-none-eabi-gcc
CXX = $(COMPILER)/arm-none-eabi-g++
OBJCOPY = $(COMPILER)/arm-none-eabi-objcopy
SIZE = $(COMPILER)/arm-none-eabi-size


VENDOR = $(TEENSY_PATH)/hardware/teensy/cores/teensy3

C_FILES := $(wildcard *.c) $(wildcard $(VENDOR)/*.c)
CPP_FILES := $(wildcard *.cpp) $(wildcard $(VENDOR)/*.cpp)

OBJS := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)

%.elf: $(OBJS) $(VENDOR)/mk20dx128.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@

-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.d *.elf *.hex
