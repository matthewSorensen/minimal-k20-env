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

   * init.c is actually pretty useless.

## Processor Differences

###Timers

###IO

	* clean, although we do want a bunch of macros to make selecting bits simpler.

###EEPROM

	* K20s have FlexRAM, not eeprom. Interface is different - implemented in teensys eeprom.c.
	* In particular, we very much don't want to get in a situation involving unaligned reads or writes.
	* This, of course, isn't an issue on an 8-bit processor, so grbl's implementation is full of single-byte
	  reads/writes.
	* Configuring the FlexRAM is quite odd, and requires executing some code from RAM.
	* In the interest of brevity, we provide two functions - aligned n-word write with 32-bit checksum, and aligned n-word read with 32-bit checksum.
	  * because we can, use the hardware checksum.
	
###USB Serial Communication

       * We want to use the K20 as a cdc device. Exact implementation is in usb_serial.c.
       * Also need to initialize the usb subsystem, which is usb_dev.c and requires usb_desc.c.
       * Both of these require usb_mem.c - oh man this is getting heavy.
       * AND usb_names

       * This is much more complicated than I want to deal with right now. Cleaning it up and specializing this is a possibility, but a
       	 clean re-implementation is pointless to address this quarter.

## Hardware Configuration

## Per-File Notes