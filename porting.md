#Porting grbl to Freescale's K20

Notes, as a possible blogpost/technical report, on porting [grbl](https://github.com/grbl/grbl) to the [Teensy 3.0 and 3.1](http://www.pjrc.com/store/teensy3.html) Freescale K20 ARM development boards.

## Toolchain

   * use original teensy toolchain - I'd like to avoid getting too intimately acquainted with arm toolchains. Well, that was the original plan, until
     I realized exactly how unsuitable the toolchain is.

   * some dependence on part of their standard library (usb serial, register locations), at least initially.

   * makefile modified for compiles outside of their tree

   * their source does need minor modifications - namely, they provide implementations of ISRs that we need
     to override. Solution is to delete them from the source (easy, small code size), or declare them as
     weak symbols (compatible with libraries that do require an ISR that we don't implement, could be pushed upstream).

   * also, the ISRs for pin interrupts. We probably need to just kill 90% of the teensy code. Ganked port c already, probably need the rest

   * this then requires a minimalist implementation of pins_teensy.c, some modifications to mk20*.c, and whatever files implement all of the USB stuff.

## Processor Differences

###Timers

###IO

###EEPROM

###USB Serial Communication

## Hardware Configuration

## Per-File Notes