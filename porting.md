#Porting grbl to Freescale's K20

Notes, as a possible blogpost/technical report, on porting [grbl](https://github.com/grbl/grbl) to the [Teensy 3.0 and 3.1](http://www.pjrc.com/store/teensy3.html) Freescale K20 ARM development boards.

## Toolchain

   The standard teensy 3.0 toolchain is setup in such a way as to be unsuitable for programming beyond the confines of the Arduino API. Although modifying grbl to use the abstraction layer provided by the teensyduino code is probably possible (and may even result in something functional), it seems to violate the spirit of grbl. Although the reference hardware platform is the prototypical Arduino, grbl very explicitly doesn't use the Arduino environment!

   * Most significant reason not to use the standard environment: they provide implementations of ISRs that we need to run our code in. Less drastic solutions include deleting them from the source (easy, reduced code size for unused ISRs), or modifying their declarations to turn them into weak symbols (compatible with libraries that do require an ISR that we don't implement, may be able to push the fix upstream).

   * However, distributing the modifications is as much work as distributing a stripped-down environment, and only gives us access to a few functions that grbl doesn't use.

   * this then requires a minimalist implementation of pins_teensy.c, some modifications to mk20*.c, and whatever files implement all of the USB stuff.

   * init.c is actually pretty useless and could be removed, or refactored into a bunch of independent routines that are directly called from main.

## Processor Differences

###Timers

###IO

  * clean, although we do want a bunch of macros to make selecting bits simpler.

###EEPROM

  * K20s have FlexRAM, not eeprom. Interface is different - implemented in teensys eeprom.c.
  * In particular, we very much don't want to get in a situation involving unaligned reads or writes.
  * This, of course, isn't an issue on an 8-bit processor, so grbl's implementation is full of single-byte reads/writes.
  * Configuring the FlexRAM is quite odd, and requires executing some code from RAM.
  * In the interest of brevity, we provide two functions - aligned *n*-word write with 32-bit checksum, and aligned *n*-word read with a checksum. The K20s have hardware support for calculating CRCs, so we use that and calculate a CRC_32C in place of grbl's original 8-bit checksum. This level of redundancy probably isn't required, given the low expected number of writes and superior reliability (32K vs 10K cycles), but the original code is paranoid enough to bother with it, so why not.
  * The restricted API will require rethinking how GRBL deals with NVM, but that's fine.
  * Documentation for the FlexRAM is confusing and hard to parse, so I'm relying on the original toolchain's function for configuring it. Documentation for the CRC module is much better - C. 30 of the reference manual.    
	
###USB Serial Communication
 
  * We want to use the K20 as a cdc device. Reference implementation is in usb_serial.c.
  * Also need to initialize the usb subsystem, which is usb_dev.c and requires usb_desc.c. This requires usb_names.c and nonstd.c, etc. Quite a large dependency set.
  * This is much more complicated than I want to deal with right now. Cleaning it up and specializing this is a possibility, but a clean re-implementation is pointless, at least during this quarter.

## Hardware Configuration
 

## Per-File Notes